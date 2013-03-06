/* Inject input events into CEGUI and handle console */

#ifndef __CEGUI_main_h_
#define __CEGUI_main_h_

#include <CEGUI.h>
#include <CEGUIWindowFactoryManager.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>
#include "CEGUI_widgets.h"
#include "INPUT_main.h"

class OGRE_main;

class CEGUI_main
{
public:
  CEGUI_main(void);
  ~CEGUI_main(void);
  static CEGUI_main *getSingletonPtr(void);

  void create(OGRE_main *Ogre);
  void createInputTestLayout();
private:
  bool mouseEvent(MouseEvent &e);
  bool keyEvent(KeyboardEvent &e);
  bool joystickEvent(JoystickEvent &e);

  static CEGUI_main *mCEGUI_main;

  OGRE_main *m_Ogre;
  CEGUI::OgreRenderer *mRenderer;
  CEGUI::Window *mEditorGuiSheet;
  CEGUI::System* m_pSystem;
};

#endif // #ifndef __CEGUI_main_h_










