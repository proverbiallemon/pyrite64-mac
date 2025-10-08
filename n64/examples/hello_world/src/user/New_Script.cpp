//#include "actor/base.h"
#include "scene/sceneManager.h"

namespace P64::Script::C52A4013240F9C81
{
  void update()
  {
    //debugf("Script A (C52A4013240F9C81)\n");

    auto pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if (pressed.a) {
      SceneManager::load(2);
    }

  }
}
