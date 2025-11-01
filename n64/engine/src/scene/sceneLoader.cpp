/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <cstdint>
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>

#include "scene/scene.h"
#include "scene/globalState.h"

#include "lib/memory.h"
#include "lib/logger.h"
#include "lib/matrixManager.h"
#include "assets/assetManager.h"
#include "scene/componentTable.h"
#include "script/scriptTable.h"

namespace {
  constexpr uint16_t OBJ_TYPE_OBJECT = 0;
  constexpr uint16_t OBJ_TYPE_CAMERA = 1;

  struct ObjectEntry {
    uint16_t type;
    uint16_t id;
    fm_vec3_t pos;
    fm_vec3_t scale;
    // data follows
  };

  struct __attribute__((packed)) ObjectEntryCamera : public ObjectEntry {
    uint16_t _padding;
    fm_vec3_t pos{};
    fm_quat_t rot{};
    float fov{};
    float near{};
    float far{};
    int16_t vpOffset[2]{};
    int16_t vpSize[2]{};
  };

  // to avoid any allocations for file names,
  // the path is stored here and changed by each load
  char scenePath[] = "rom:/p64/s0000_";

  inline void* loadSubFile(char type) {
    scenePath[sizeof(scenePath)-2] = type;
    scenePath[sizeof(scenePath)-1] = '\0';
    return asset_load(scenePath, nullptr);
  }
}

void P64::Scene::loadScene() {
  scenePath[sizeof(scenePath)-3] = '0' + id;
  scenePath[sizeof(scenePath)-2] = '\0';

  {
    // @TODO: load into
    auto *tmp = (SceneConf*)asset_load(scenePath, nullptr);
    conf = *tmp;
    free(tmp);
  }

  cameras.clear();

  stringTable = (char*)loadSubFile('s');

  //debugf("Objects: %lu\n", conf.objectCount);
  if(conf.objectCount)
  {
/*
    objStaticMats = (T3DMat4FP*)(loadSubFile('m'));
    auto currStaticMats = objStaticMats;
    data_cache_hit_writeback(objStaticMats, conf.objectCount * sizeof(T3DMat4FP));
*/
    auto *objFileStart = (uint8_t*)(loadSubFile('o'));

    // pre-scan objects to get counts
    auto *objFile = objFileStart;
    //cameras.resize(camCount);

    // now process all other objects
    objFile = objFileStart;
    for(uint32_t i=0; i<conf.objectCount; ++i)
    {
      ObjectEntry* objEntry = (ObjectEntry*)objFile;

      //debugf("OBJECT: id=%d type=%d\n", objEntry->id, objEntry->type);

      switch (objEntry->type)
      {
        case OBJ_TYPE_OBJECT: {

          // pre-scan components to get total allocation size
          uint32_t allocSize = sizeof(Object);
          auto ptrIn = objFile + sizeof(ObjectEntry);
          uint32_t compCount = 0;
          while(ptrIn[1] != 0) {
            auto compId = ptrIn[0];
            auto argSize = ptrIn[1] * 4;

            const auto &compDef = COMP_TABLE[compId];
            assertf(compDef.getAllocSize != nullptr, "Component %d unknown!", compId);
            allocSize += compDef.getAllocSize(ptrIn + 4);
            allocSize += sizeof(Object::CompRef);

            ptrIn += argSize;
            ++compCount;
          }

          void* objMem = malloc(allocSize); // @TODO: custom allocator
          auto objCompTablePtr = (Object::CompRef*)((char*)objMem + sizeof(Object));
          auto objCompDataPtr = (char*)(objCompTablePtr) + (sizeof(Object::CompRef) * compCount);

          Object* obj = new(objMem) Object();
          obj->id = objEntry->id;
          obj->compCount = compCount;
          obj->pos = objEntry->pos;
          obj->scale = objEntry->scale;

          ptrIn = objFile + sizeof(ObjectEntry);
          while(ptrIn[1] != 0)
          {
            uint8_t compId = ptrIn[0];
            uint8_t argSize = ptrIn[1] * 4;

            const auto &compDef = COMP_TABLE[compId];
            //debugf("Alloc: %lu bytes for comp %d (arg: %d)\n", compDef.allocSize, compId, argSize);

            objCompTablePtr->type = compId;
            objCompTablePtr->flags = 0;
            objCompTablePtr->offset = objCompDataPtr - (char*)obj;
            ++objCompTablePtr;

            compDef.initDel(*obj, objCompDataPtr, ptrIn + 4);
            objCompDataPtr += compDef.getAllocSize(ptrIn + 4);
            ptrIn += argSize;
          }

          objects.push_back(obj);
          objFile = ptrIn + 4;

        } break;
/*
        case OBJ_TYPE_CAMERA: {
          auto* objCam = (ObjectEntryCamera*)objEntry;
          auto &cam = cameras[camIdx++];
          cam.setPos(objCam->pos);
          cam.fov  = objCam->fov;
          cam.near = objCam->near;
          cam.far  = objCam->far;
          for(auto &vp : cam.viewports)
          {
            vp = t3d_viewport_create();
            t3d_viewport_set_area(vp,
              objCam->vpOffset[0], objCam->vpOffset[1],
              objCam->vpSize[0], objCam->vpSize[1]
            );
          }
        } break;
*/
        default:
          debugf("Unknown object type: %04X\n", objEntry->type);
          break;
      }

    }

    free(objFileStart);
    assert(cameras.size() != 0);
  }
}
