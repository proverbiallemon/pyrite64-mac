/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include <libdragon.h>
#include <cstdint>
#include <malloc.h>
#include "scene/scene.h"
#include "lib/math.h"
#include "scene/componentTable.h"

namespace {
  constexpr uint32_t DATA_ALIGN = 8;

  struct ObjectEntry {
    uint16_t flags;
    uint16_t id;
    uint16_t group;
    uint16_t _padding;
    fm_vec3_t pos;
    fm_vec3_t scale;
    uint32_t packedRot;
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

void P64::Scene::loadSceneConfig()
{
  scenePath[sizeof(scenePath)-3] = '0' + id;
  scenePath[sizeof(scenePath)-2] = '\0';

  {
    auto *tmp = (SceneConf*)asset_load(scenePath, nullptr);
    conf = *tmp;
    free(tmp);
  }
}

void P64::Scene::loadScene() {
  scenePath[sizeof(scenePath)-3] = '0' + id;
  scenePath[sizeof(scenePath)-2] = '\0';

  cameras.clear();

  //debugf("Objects: %lu\n", conf.objectCount);
  if(conf.objectCount)
  {
    auto *objFileStart = (uint8_t*)(loadSubFile('o'));

    auto *objFile = objFileStart;

    // now process all other objects
    objFile = objFileStart;
    for(uint32_t i=0; i<conf.objectCount; ++i)
    {
      ObjectEntry* objEntry = (ObjectEntry*)objFile;

      // pre-scan components to get total allocation size
      uint32_t allocSize = sizeof(Object);

      // some alignment logic below relies on an at a minimum 4-byte size
      static_assert(sizeof(Object) % 4 == 0);
      static_assert(sizeof(Object::CompRef) % 4 == 0);

      auto ptrIn = objFile + sizeof(ObjectEntry);
      uint32_t compCount = 0;
      uint32_t compDataSize = 0;
      while(ptrIn[1] != 0) {
        auto compId = ptrIn[0];
        auto argSize = ptrIn[1] * 4;

        const auto &compDef = COMP_TABLE[compId];
        assertf(compDef.getAllocSize != nullptr, "Component %d unknown!", compId);
        compDataSize += Math::alignUp(compDef.getAllocSize(ptrIn + 4), DATA_ALIGN);
        allocSize += sizeof(Object::CompRef);

        ptrIn += argSize;
        ++compCount;
      }

      // component data must be 8-byte aligned, GCC tries to be smart
      // and some structs cuse 64-bit writes to members.
      // if it is misaligned, add spacing after the comp table
      uint32_t offsetData = (sizeof(Object::CompRef) * compCount);
      if(allocSize % 8 != 0) {
        compDataSize += 4;
        offsetData += 4;
      }

      allocSize += compDataSize;

      //debugf("Allocating object %d | comps: %d | size: %lu bytes\n", objEntry->id, compCount, allocSize);

      void* objMem = memalign(DATA_ALIGN, allocSize); // @TODO: custom allocator

      auto objCompTablePtr = (Object::CompRef*)((char*)objMem + sizeof(Object));
      auto objCompDataPtr = (char*)(objCompTablePtr) + offsetData;

      Object* obj = new(objMem) Object();
      obj->id = objEntry->id;
      obj->group = objEntry->group;
      obj->flags = objEntry->flags;
      obj->compCount = compCount;
      obj->pos = objEntry->pos;
      obj->scale = objEntry->scale;
      obj->rot = Math::unpackQuat(objEntry->packedRot);

      ptrIn = objFile + sizeof(ObjectEntry);
      while(ptrIn[1] != 0)
      {
        uint8_t compId = ptrIn[0];
        uint8_t argSize = ptrIn[1] * 4;

        const auto &compDef = COMP_TABLE[compId];
        // debugf("Alloc: comp %d (arg: %d)\n", compId, argSize);

        objCompTablePtr->type = compId;
        objCompTablePtr->flags = 0;
        objCompTablePtr->offset = objCompDataPtr - (char*)obj;
        ++objCompTablePtr;

        compDef.initDel(*obj, objCompDataPtr, ptrIn + 4);
        objCompDataPtr += Math::alignUp(compDef.getAllocSize(ptrIn + 4), 8);
        ptrIn += argSize;
      }

      objects.push_back(obj);
      objFile = ptrIn + 4;
    }

    free(objFileStart);
  }

  // update groups
  for(auto obj : objects)
  {
    if(obj->isGroup())
    {
      bool groupActive = obj->isSelfEnabled();
      debugf("Updating group %d | a:%d\n", obj->id, groupActive);
      setGroupEnabled(obj->id, groupActive);
    }
  }
}
