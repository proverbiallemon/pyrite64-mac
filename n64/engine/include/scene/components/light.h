/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <t3d/t3dmodel.h>

#include "assets/assetManager.h"
#include "lib/matrixManager.h"
#include "scene/object.h"
#include "scene/sceneManager.h"
#include "script/scriptTable.h"

namespace P64::Comp
{
  struct Light
  {
    struct InitData
    {
      color_t color;
      uint8_t index;
      uint8_t type;
      int8_t dir[3];
    };

    fm_vec3_t dir{};
    color_t color{};
    uint8_t type{};
    uint8_t index{};


    static uint32_t getAllocSize(InitData* initData)
    {
      return sizeof(Light);
    }

    static void initDelete(Object& obj, Light* data, InitData* initData)
    {
      if (initData == nullptr) {
        data->~Light();
        return;
      }

      new(data) Light();
      data->color = initData->color;
      data->type = initData->type;
      data->index = initData->index;
      data->dir = {
        (float)initData->dir[0] * (1.0f / 127.0f),
        (float)initData->dir[1] * (1.0f / 127.0f),
        (float)initData->dir[2] * (1.0f / 127.0f)
      };
    }

    static void update(Object& obj, Light* data) {
      auto &light = SceneManager::getCurrent().getLighting();
      if (data->type == 0) {
        light.addAmbientLight(data->color);
      } else {
        light.addDirLight(data->color, data->dir);
      }
    }

    static void draw(Object& obj, Light* data) {

    }
  };
}