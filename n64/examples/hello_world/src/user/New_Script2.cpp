//#include "actor/base.h"
#include "scene/sceneManager.h"

namespace P64::Script::C3AF5D870988CBC0
{
  void update()
  {
   //
    auto pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if (pressed.b) {
      debugf("Script B (C3AF5D870988CBC0)\n");
      SceneManager::load(1);
    }
  }
}
