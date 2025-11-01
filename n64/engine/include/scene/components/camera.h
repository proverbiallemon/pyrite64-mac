/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once

#include "assets/assetManager.h"
#include "scene/object.h"
#include "scene/sceneManager.h"

namespace P64::Comp
{
  struct Camera
  {
    struct InitData
    {
      int vpOffset[2];
      int vpSize[2];
      float fov;
      float near;
      float far;
    };

    fm_vec3_t dir{};
    color_t color{};
    uint8_t type{};
    uint8_t index{};


    static uint32_t getAllocSize(InitData* initData)
    {
      return sizeof(Camera);
    }

    static void initDelete(Object& obj, Camera* data, InitData* initData)
    {
      if (initData == nullptr) {
        data->~Camera();
        return;
      }

      new(data) Camera();

      auto &cam = SceneManager::getCurrent().addCamera();
      cam.setPos(obj.pos);
      cam.fov  = initData->fov;
      cam.near = initData->near;
      cam.far  = initData->far;
      for(auto &vp : cam.viewports) {
        vp = t3d_viewport_create();
        t3d_viewport_set_area(vp,
          initData->vpOffset[0], initData->vpOffset[1],
          initData->vpSize[0], initData->vpSize[1]
        );
      }
    }

    static void update(Object& obj, Camera* data) {
    }

    static void draw(Object& obj, Camera* data) {
    }
  };
}