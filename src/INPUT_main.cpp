#include "Log.h"
#include "INPUT_main.h"
extern "C" {
#include <hidsdi.h>
}

INPUT_main *INPUT_main::mINPUT_main;
 
INPUT_main::INPUT_main(void) : numJoyButtons(0) {
}
 
INPUT_main::~INPUT_main(void) {
}

boost::signals2::connection INPUT_main::connect_mouse(const signal_mouse_t::slot_type &subscriber)
{
  return m_mouse_sig.connect(subscriber);
}

boost::signals2::connection INPUT_main::connect_keyboard(const signal_keyboard_t::slot_type &subscriber)
{
  return m_keyboard_sig.connect(subscriber);
}

boost::signals2::connection INPUT_main::connect_joystick(const signal_joystick_t::slot_type &subscriber)
{
  return m_joystick_sig.connect(subscriber);
}
 
void INPUT_main::initialise(Ogre::RenderWindow *renderWindow) {
  // Setup basic variables
  size_t windowHnd = 0;
  std::ostringstream windowHndStr;
 
  // Get window handle
  renderWindow->getCustomAttribute("WINDOW", &windowHnd);
 
  RAWINPUTDEVICE Rid[3];
  // Keyboard
  Rid[0].usUsagePage = 1;
  Rid[0].usUsage     = 6;
  Rid[0].dwFlags     = 0; //RIDEV_NOLEGACY; // no need for legacy messages?
  Rid[0].hwndTarget  = NULL;
  // Mouse
  Rid[1].usUsagePage = 1;
  Rid[1].usUsage     = 2;
  Rid[1].dwFlags     = 0; //RIDEV_NOLEGACY;
  Rid[1].hwndTarget  = NULL;
  // Joystick
  Rid[2].usUsagePage = 1;
  Rid[2].usUsage     = 4;
  Rid[2].dwFlags     = 0;
  Rid[2].hwndTarget  = NULL;
  if (RegisterRawInputDevices(Rid, 3, sizeof(RAWINPUTDEVICE))==FALSE) {
    FILE_LOG(logERROR) << "Unable to register raw input devices!";
    exit(EXIT_FAILURE);
  }

  // Get window size
  unsigned int width, height, depth;
  int left, top;
  renderWindow->getMetrics(width, height, depth, left, top);
 
  // Set mouse region
  this->setWindowExtents(width, height);
}
 
void INPUT_main::capture(void) {
  // Need to capture / update each device every frame
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
JoystickEvent GetJoyStickEvent(PRAWINPUT pRawInput, int &numJoyButtons) {
  JoystickEvent je;
#define CHECK(exp)      { if(!(exp)) goto Error; }
#define SAFE_FREE(p)    { if(p) { HeapFree(hHeap, 0, p); (p) = NULL; } }

  PHIDP_PREPARSED_DATA pPreparsedData;
  HIDP_CAPS            Caps;
  PHIDP_BUTTON_CAPS    pButtonCaps;
  PHIDP_VALUE_CAPS     pValueCaps;
  USHORT               capsLength;
  UINT                 bufferSize;
  HANDLE               hHeap;
  USAGE                usage[MAX_JOY_BUTTONS];
  ULONG                i, usageLength, value;

  pPreparsedData = NULL;
  pButtonCaps    = NULL;
  pValueCaps     = NULL;
  hHeap          = GetProcessHeap();

  // Get Preparsed data block
  CHECK( GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize) == 0 );
  CHECK( pPreparsedData = (PHIDP_PREPARSED_DATA)HeapAlloc(hHeap, 0, bufferSize) );
  CHECK( (int)GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &bufferSize) >= 0 );

  // Button caps
  CHECK( HidP_GetCaps(pPreparsedData, &Caps) == HIDP_STATUS_SUCCESS );
  CHECK( pButtonCaps = (PHIDP_BUTTON_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_BUTTON_CAPS) * Caps.NumberInputButtonCaps) );

  capsLength = Caps.NumberInputButtonCaps;
  CHECK( HidP_GetButtonCaps(HidP_Input, pButtonCaps, &capsLength, pPreparsedData) == HIDP_STATUS_SUCCESS )
    numJoyButtons = pButtonCaps->Range.UsageMax - pButtonCaps->Range.UsageMin + 1;

  je.numJoyButtons = numJoyButtons;

  // Value caps
  CHECK( pValueCaps = (PHIDP_VALUE_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_VALUE_CAPS) * Caps.NumberInputValueCaps) );
  capsLength = Caps.NumberInputValueCaps;
  CHECK( HidP_GetValueCaps(HidP_Input, pValueCaps, &capsLength, pPreparsedData) == HIDP_STATUS_SUCCESS );

  // Get the pressed buttons
  usageLength = numJoyButtons;
  CHECK(
    HidP_GetUsages(
      HidP_Input, pButtonCaps->UsagePage, 0, usage, &usageLength, pPreparsedData,
      (PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid
      ) == HIDP_STATUS_SUCCESS );

  ZeroMemory(je.bButtonStates, sizeof(je.bButtonStates));
  for(i = 0; i < usageLength; i++)
    je.bButtonStates[usage[i] - pButtonCaps->Range.UsageMin] = TRUE;

  // Get the state of discrete-valued-controls
  for(i = 0; i < Caps.NumberInputValueCaps; i++)
  {
    CHECK(
      HidP_GetUsageValue(
        HidP_Input, pValueCaps[i].UsagePage, 0, pValueCaps[i].Range.UsageMin, &value, pPreparsedData,
        (PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid
        ) == HIDP_STATUS_SUCCESS );

    switch(pValueCaps[i].Range.UsageMin)
    {
    case 0x30:      // X-axis
      je.lAxisX = (LONG)value - 128;
      break;
    case 0x31:      // Y-axis
      je.lAxisY = (LONG)value - 128;
      break;
    case 0x32:      // Z-axis
      je.lAxisZ = (LONG)value - 128;
      break;
    case 0x35:      // Rotate-Z
      je.lAxisRz = (LONG)value - 128;
      break;
    case 0x39:      // Hat Switch
      je.lHat = value;
      break;
    }
  }
  // Clean up
Error:
  SAFE_FREE(pPreparsedData);
  SAFE_FREE(pButtonCaps);
  SAFE_FREE(pValueCaps);
#undef CHECK
#undef SAFE_FREE

  return je;
}

void INPUT_main::inputEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_INPUT) {
    if (wParam == RIM_INPUT) { // event occured with window in foreground
      PRAWINPUT pRawInput;
      UINT      bufferSize;
      HANDLE    hHeap;

      GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));
      hHeap     = GetProcessHeap();
      pRawInput = (PRAWINPUT)HeapAlloc(hHeap, 0, bufferSize);
      if(!pRawInput)
        return;
      GetRawInputData((HRAWINPUT)lParam, RID_INPUT, pRawInput, &bufferSize, sizeof(RAWINPUTHEADER));

      if (pRawInput->header.dwType == RIM_TYPEMOUSE)
      {
        // fill out a mouseevent struct
        MouseEvent me;
        me.x = pRawInput->data.mouse.lLastX;
        me.y = pRawInput->data.mouse.lLastY;
        me.buttons = 0;
        me.wheel = 0;
        me.mouse_leaving_client = 0;

        if (pRawInput->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
          me.buttons = me.buttons | 1L << MB_Left;
        if (pRawInput->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
          me.buttons = me.buttons | 1L << MB_Middle;
        if (pRawInput->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
          me.buttons = me.buttons | 1L << MB_Right;
        if (pRawInput->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
          me.buttons = me.buttons | 1L << MB_Button3;
        if (pRawInput->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
          me.buttons = me.buttons | 1L << MB_Button4;
        if (pRawInput->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
          me.wheel = (SHORT)pRawInput->data.mouse.usButtonData; // positive delta (120) is up

        if (pRawInput->data.mouse.usFlags & 0x01 == MOUSE_MOVE_ABSOLUTE)
          me.buttons = me.buttons | 1L << MB_RELATIVE_OR_ABSOLUTE_XY;

        mouseEvent(me);
      }
      else if (pRawInput->header.dwType == RIM_TYPEKEYBOARD)
      {
        KeyboardEvent ke;
        ke.keycode  = pRawInput->data.keyboard.MakeCode;
        ke.flags    = pRawInput->data.keyboard.Flags;
        ke.vkeycode = pRawInput->data.keyboard.VKey;

        keyEvent(ke);
      }
      else if (pRawInput->header.dwType == RIM_TYPEHID)
      {
        joystickEvent(GetJoyStickEvent(pRawInput, numJoyButtons));
      }

      HeapFree(hHeap, 0, pRawInput);
    }
  } // WM_INPUT
  if (uMsg == WM_MOUSELEAVE)
  {
    MouseEvent e;
    e.buttons = 0;
    e.wheel = 0;
    e.mouse_leaving_client = 1;

    mouseEvent(e);
  }
}
#endif
 
void INPUT_main::setWindowExtents(int width, int height) {
  // Set mouse region (if window resizes, we should alter this to reflect as well)
}
 
int INPUT_main::getNumOfJoysticks(void) {
  return 1; // TODO
}
 
bool INPUT_main::mouseEvent(MouseEvent &e) {
  //FILE_LOG(logINFO) << "mouseEvent: " << e.x << "," << e.y << " btns:" << std::hex << e.buttons << " wheel:" << std::dec << e.wheel;
  m_mouse_sig(e);
  return true;
}

bool INPUT_main::keyEvent(KeyboardEvent &e) {
  //FILE_LOG(logINFO) << "keyEvent: " << e.keycode << " flags:" << e.flags << " vkey:" << e.vkeycode << "  " << VkKeyScan('k');
  m_keyboard_sig(e); 
  return true;
}

bool INPUT_main::joystickEvent(JoystickEvent &e) {
  /*
    char buf[256] = "";
    for(int i = 0; i < numJoyButtons; i++) {
    sprintf(buf+i*2, "%d ", e.bButtonStates[i]);
    }
    FILE_LOG(logINFO) << "num:" << numJoyButtons << " joystickEvent - xy:" << e.lAxisX << "," << e.lAxisY << " zr:" << e.lAxisZ << "," << e.lAxisRz << " hat:" << e.lHat << " buttons: " << buf;
  */
  m_joystick_sig(e);
  return true;
}
 
INPUT_main* INPUT_main::getSingletonPtr(void) {
  if(!mINPUT_main) {
    mINPUT_main = new INPUT_main();
  }
 
  return mINPUT_main;
}
