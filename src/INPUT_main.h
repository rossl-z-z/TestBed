/* Main for input: mouse, keyboard, joystick/gamepad */

// windows: using RawInput API (see windows api) requires window handle

#ifndef __INPUT_main_H
#define __INPUT_main_H

#include <OgreRenderWindow.h>
#include <boost/signals2/signal.hpp>

#define MAX_JOY_BUTTONS		128

enum MouseButtonID
{
  MB_Left = 0, MB_Right, MB_Middle,
  MB_Button3, MB_Button4, MB_Button5, MB_Button6, MB_Button7,
  MB_WheelMove, MB_RELATIVE_OR_ABSOLUTE_XY
};

struct MouseEvent
{
  int x;
  int y;
  int buttons;
  int wheel;   // waste of space? fit in buttons?
  int mouse_leaving_client;

  inline bool buttonDown( MouseButtonID button ) const
  {
    return ((buttons & ( 1L << button )) == 0) ? false : true;
  }
};

struct KeyboardEvent
{
  unsigned short keycode;
  unsigned short flags;  // RI_KEY__BREAK=1 is up, 2 is left version of the key, 3 is right, 0 is key down
  unsigned short vkeycode;
};

struct JoystickEvent
{
  int numJoyButtons;
  bool bButtonStates[MAX_JOY_BUTTONS];
  int lAxisX;
  int lAxisY;
  int lAxisZ;
  int lAxisRz;
  int lHat;
};

class INPUT_main {
public:
  virtual ~INPUT_main(void);

  typedef boost::signals2::signal<void (MouseEvent &me)>  signal_mouse_t;
  boost::signals2::connection connect_mouse(const signal_mouse_t::slot_type &subscriber);
  typedef boost::signals2::signal<void (KeyboardEvent &me)>  signal_keyboard_t;
  boost::signals2::connection connect_keyboard(const signal_keyboard_t::slot_type &subscriber);
  typedef boost::signals2::signal<void (JoystickEvent &me)>  signal_joystick_t;
  boost::signals2::connection connect_joystick(const signal_joystick_t::slot_type &subscriber);
  
  void initialise(Ogre::RenderWindow *renderWindow);
  void capture(void); // capture / update each device every frame

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  void inputEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
 
  void setWindowExtents(int width, int height); // update for mouse movement limits
  int getNumOfJoysticks(void);
  static INPUT_main* getSingletonPtr(void);
private:
  INPUT_main(void);
  INPUT_main(const INPUT_main&) {}
  INPUT_main & operator = (const INPUT_main&);
  
  bool mouseEvent(MouseEvent &e);
  bool keyEvent(KeyboardEvent &e);
  bool joystickEvent(JoystickEvent &e);

  signal_mouse_t    m_mouse_sig;
  signal_keyboard_t m_keyboard_sig;
  signal_joystick_t m_joystick_sig;

  int numJoyButtons;

  static INPUT_main *mINPUT_main;
};
#endif // #ifndef __INPUT_main_H 
