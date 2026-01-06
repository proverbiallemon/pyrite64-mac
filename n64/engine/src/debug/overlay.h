/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include <t3d/t3dmath.h>
#include <vector>
#include <functional>

#include "scene/scene.h"

namespace Debug
{
  class Overlay
  {
    public:
      enum class MenuItemType : uint8_t {
        BOOL,
        INT,
        ACTION
      };
      struct MenuItem {
        const char *text{};
        int value{};
        MenuItemType type{};
        std::function<void(MenuItem&)> onChange{};
      };

      struct Menu {
        std::vector<MenuItem> items{};
        uint32_t currIndex;
      };

    private:
      Menu menu{};
      Menu menuScenes{};

    public:
      void setEnabled(bool enabled);
      void draw(P64::Scene &scene, int triCount, float deltaTime);
  };
}
