/**
* @copyright 2026 - Max Bebök
* @license MIT
*/
#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "tiny-regex-c/re.h"

#include "../../../utils/prop.h"
#include "tiny3d/tools/gltf_importer/src/structs.h"

namespace Project
{
  class Object;
}

namespace Project::Component::Shared
{
  struct MeshFilter
  {
    PROP_STRING(meshFilter);
    std::vector<uint32_t> cache{};

    const std::vector<uint32_t>& filterT3DM(const std::vector<T3DM::Model> &models, Object& obj, bool withMaterial);
  };
}
