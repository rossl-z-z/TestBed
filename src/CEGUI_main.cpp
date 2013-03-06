#include "Log.h"
#include "CEGUI_main.h"
#include "CEGUI_keytranslate.h"
#include "OgreResourceGroupManager.h"
#include "input_test_layout.h"

CEGUI_main *CEGUI_main::mCEGUI_main;

CEGUI_main::CEGUI_main(void)
  : mRenderer(0),
    mEditorGuiSheet(0),
    m_pSystem(NULL)
{
}

CEGUI_main::~CEGUI_main(void)
{
  if(mEditorGuiSheet)
  {
    CEGUI::WindowManager::getSingleton().destroyWindow(mEditorGuiSheet);
  }
}

void CEGUI_main::create(OGRE_main *Ogre)
{
  m_Ogre = Ogre;
  mRenderer = &CEGUI::OgreRenderer::bootstrapSystem(*m_Ogre->getRenderWindow());
  
  // create the root CEGUI class
  m_pSystem = CEGUI::System::getSingletonPtr();
  //CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

  // register the new window types
  CEGUI::WindowFactoryManager::getSingleton().
      addFactory<CEGUI::TplWindowFactory<AxisIndicator> >();
  CEGUI::WindowFactoryManager::getSingleton().
      addFactory<CEGUI::TplWindowFactory<HatIndicator> >();

  CEGUI::SchemeManager::getSingleton().create((CEGUI::utf8*)"EggShell.scheme", (CEGUI::utf8*)"GUI");
  m_pSystem->setDefaultMouseCursor((CEGUI::utf8*)"EggShellImages", (CEGUI::utf8*)"MouseArrow");

  // use this font for text in the UI
  CEGUI::FontManager::getSingleton().create("Tahoma-8.font", (CEGUI::utf8*)"GUI");
  m_pSystem->setDefaultFont((CEGUI::utf8*)"Tahoma-8");

  // setup input
  INPUT_main::getSingletonPtr()->connect_mouse(boost::bind(&CEGUI_main::mouseEvent, this, _1));
  INPUT_main::getSingletonPtr()->connect_keyboard(boost::bind(&CEGUI_main::keyEvent, this, _1));
  INPUT_main::getSingletonPtr()->connect_joystick(boost::bind(&CEGUI_main::joystickEvent, this, _1));
  
  createInputTestLayout();
}

void CEGUI_main::createInputTestLayout()
{
  // use input_test.layout to show different inputs: mouse, keyboard, joystick
  CEGUI::Window* pLayout = CEGUI::WindowManager::getSingleton().loadWindowLayout("input_test.layout", "", "GUI");
  m_pSystem->setGUISheet(pLayout);

  // input_test.layout handler
  input_test_layout* pLayout1 = new input_test_layout(m_pSystem, pLayout, m_Ogre);
}

bool CEGUI_main::mouseEvent(MouseEvent &e)
{
  static int prev_buttons = 0; // track previous buttons so we know when they change

	m_pSystem->injectMouseWheelChange((float)e.wheel);
  if (e.buttonDown(MB_RELATIVE_OR_ABSOLUTE_XY))
	  m_pSystem->injectMousePosition(e.x, e.y);
  else
    m_pSystem->injectMouseMove(e.x, e.y);

  if (e.mouse_leaving_client == 1) {
    m_pSystem->injectMouseLeaves();
  } else {
  int changed = prev_buttons ^ e.buttons;
  for (int i = MB_Left; i < MB_Button4; i++)
  {
    if (changed & ( 1L << i ) ) {
      if (e.buttonDown((MouseButtonID)i)) // these 2 MouseButton structs are close enough
        m_pSystem->injectMouseButtonDown((CEGUI::MouseButton)i);
      else
        m_pSystem->injectMouseButtonUp((CEGUI::MouseButton)i);
    }
  }
  prev_buttons = e.buttons;
  }

  return true;
}

bool CEGUI_main::keyEvent(KeyboardEvent &e)
{
  HKL layout=GetKeyboardLayout(0); // TODO: different languages?

  static bool isShiftDown = false;

  if (e.flags) {
    if (e.keycode == 0x36)
      isShiftDown = false;
    m_pSystem->injectKeyUp(e.keycode);
  }
  else {
    if (e.keycode == 0x36)
      isShiftDown = true;
    m_pSystem->injectKeyDown(e.keycode);
    char ch = translate_scan_code(e.keycode, isShiftDown);
    if (ch)
      m_pSystem->injectChar(ch);
  }
  
  return true;
}

bool CEGUI_main::joystickEvent(JoystickEvent &e)
{
  // no reason to use here, but if you want a virtual mouse cursor... maybe?
  return true;
}

CEGUI_main* CEGUI_main::getSingletonPtr(void)
{
  if(!mCEGUI_main) {
    mCEGUI_main = new CEGUI_main();
  }

  return mCEGUI_main;
}
