#ifndef __input_test_layout_h_
#define __input_test_layout_h_
 
#include "CEGUISystem.h"
#include "CEGUIWindow.h"
#include "CEGUIWindowManager.h"
#include "elements/CEGUIPushButton.h"
#include "OGRE_main.h"
#include "INPUT_main.h"
#include "CEGUI_widgets.h"

class input_test_layout
{
public:
  input_test_layout(CEGUI::System* pSystem, CEGUI::Window* pSheet, OGRE_main *ogre)
  {
	  m_pSystem = pSystem;
	  m_pWindow = pSheet;
    m_Ogre = ogre;
 
	  // hook up the event handlers to the window elements
	  CEGUI::PushButton* pQuitButton = (CEGUI::PushButton *)CEGUI::WindowManager::getSingleton().getWindow("cmdQuit");
	  pQuitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&input_test_layout::Quit_OnClick, this));

    CEGUI::Editbox* pEditbox = (CEGUI::Editbox *)CEGUI::WindowManager::getSingleton().getWindow("KeyboardText");
    pEditbox->subscribeEvent(CEGUI::Editbox::EventCharacterKey, CEGUI::Event::Subscriber(&input_test_layout::KeyPushed, this));

    // joystick INPUT
    INPUT_main::getSingletonPtr()->connect_joystick(boost::bind(&input_test_layout::joystickEvent, this, _1));
  }
  ~input_test_layout() {}
 
	// signature: bool <method name>(const CEGUI::EventArgs &args)
	bool Quit_OnClick(const CEGUI::EventArgs &args)
  {
    m_Ogre->exit(); // using OGRE_main requires it to be fully defined, so only include layouts in CEGUI_main.cpp
	  return true;
  }
  bool KeyPushed(const CEGUI::EventArgs &args)
  {
    // TODO: make sure keyboardtext control always has focus?
    CEGUI::Editbox* pEditbox = (CEGUI::Editbox *)CEGUI::WindowManager::getSingleton().getWindow("KeyboardText");
    CEGUI::String tmp = pEditbox->getText();
    if (tmp.length() > 4) // erase start of text such that max length is 5 chars
      tmp.erase(tmp.begin(), tmp.begin()+(tmp.length()-4));
    pEditbox->setText(tmp);
	  return false;  // report unhandled event, so that characters are added
  }

  bool joystickEvent(JoystickEvent &e)
  {
    const int MID_VALUE = 128; // TODO: joystick calibration?
    const int MAX_VALUE = 256;
    char buf[32];
    for (int i = 0; i < e.numJoyButtons; i++) {
      sprintf(buf, "Joy%d", i+1);
      CEGUI::PushButton* pButton = (CEGUI::PushButton *)CEGUI::WindowManager::getSingleton().getWindow(buf);
      if (e.bButtonStates[i]) {
        pButton->disable();
      } else {
        pButton->enable();
      }
    }
    for (int i = 0; i < 2; i++) {
      sprintf(buf, "JoyAxis%d", i+1);
      AxisIndicator* pAxis = (AxisIndicator *)CEGUI::WindowManager::getSingleton().getWindow(buf);
      if (i == 0)
        pAxis->setAxisPosition(CEGUI::UVector2(CEGUI::UDim(((float)e.lAxisX+MID_VALUE)/MAX_VALUE, -1), CEGUI::UDim(((float)e.lAxisY+MID_VALUE)/MAX_VALUE, -1)));
      else
        pAxis->setAxisPosition(CEGUI::UVector2(CEGUI::UDim(((float)e.lAxisZ+MID_VALUE)/MAX_VALUE, -1),CEGUI::UDim(((float)e.lAxisRz+MID_VALUE)/MAX_VALUE, -1)));
      pAxis->invalidate();
    }
    sprintf(buf, "JoyHat");
    HatIndicator* pHat = (HatIndicator *)CEGUI::WindowManager::getSingleton().getWindow(buf);
    pHat->setHatPosition(e.lHat);
    pHat->invalidate();

    return true;
  }
  
private:
	CEGUI::System* m_pSystem;	// pointer to the CEGUI System instance
	CEGUI::Window* m_pWindow;	// pointer to the layout sheet window
  OGRE_main*     m_Ogre;    // pointer to main app
};

#endif // #ifndef __input_test_layout_h_