/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <fgeom.h>
#include <graphics.h>

namespace P64
{
  constexpr uint32_t MAX_LIGHTS = 6;

  struct Light
  {
    fm_vec3_t dirOrPos{};
    float strength{};
    color_t color{};
  };

  class Lighting
  {
    private:
      uint32_t lightCount{0};
      Light lights[MAX_LIGHTS]{};

      void addLight(const Light& l) {
        if(lightCount >= MAX_LIGHTS)return;
        lights[lightCount++] = l;
      }

    public:
      void reset() {
        lightCount = 0;
      }

      void apply() const;

      void addAmbientLight(const color_t col) {
        addLight({.strength = -1, .color = col});
      }

      void addDirLight(const color_t col, const fm_vec3_t& dir) {
        addLight({.dirOrPos = dir, .color = col});
      }

      void addPointLight(const color_t col, const fm_vec3_t& pos, float strength) {
        strength = fmaxf(strength, 0.001f);
        strength = fminf(strength, 1.0f);
        addLight({.dirOrPos = pos, .strength = strength, .color = col});
      }
  };
}
