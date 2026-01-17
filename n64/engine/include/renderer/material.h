/**
* @copyright 2026 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace P64::Renderer
{
  struct Material
  {
    constexpr static uint16_t MASK_DEPTH  = 1 << 0;
    constexpr static uint16_t MASK_PRIM   = 1 << 1;
    constexpr static uint16_t MASK_ENV    = 1 << 2;
    constexpr static uint16_t MASK_LIGHT  = 1 << 3;

    uint16_t setMask{};
    uint16_t valFlags{};
    color_t colorPrim{};
    color_t colorEnv{};

    [[nodiscard]] constexpr bool doesAnything() const {
      return setMask != 0;
    }

    constexpr uint16_t getDepthRead() const {
      return valFlags & 0b01;
    }

    constexpr uint16_t getDepthWrite() const {
      return valFlags & 0b10;
    }

    void begin()
    {
      if(!doesAnything())return;

      if(setMask & MASK_DEPTH) {
        rdpq_sync_pipe();
        rdpq_mode_push();
        rdpq_mode_zbuf(getDepthRead(), getDepthWrite());
      }
      if(setMask & MASK_PRIM) {
        rdpq_set_prim_color(colorPrim);
      }
      if(setMask & MASK_ENV) {
        rdpq_sync_pipe();
        rdpq_set_env_color(colorEnv);
      }
    }

    void end()
    {
      if(setMask & MASK_DEPTH)
      {
        rdpq_sync_pipe();
        rdpq_mode_pop();
      }
    }
  };
}