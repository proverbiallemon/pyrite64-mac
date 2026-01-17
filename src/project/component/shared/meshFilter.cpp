/**
* @copyright 2026 - Max Bebök
* @license MIT
*/
#include "meshFilter.h"

#include "../../scene/object.h"

namespace Project::Component::Shared
{
  const std::vector<uint32_t>& MeshFilter::filterT3DM(
    const std::vector<T3DM::Model> &models,
    Object& obj,
    bool withMaterial
  ) {
    cache.clear();
    auto &filter = meshFilter.resolve(obj.propOverrides);
    if(filter.empty())return cache;

    auto pattern = re_compile(filter.c_str());

    uint32_t idx = 0;
    for(auto &mesh : models)
    {
      auto fullName = mesh.name;
      if(withMaterial) {
        fullName += "@" + mesh.material.name;
      }

      int match_length{};
      int match_idx = re_matchp(pattern, fullName.c_str(), &match_length);
      if (match_idx != -1) {
        cache.push_back(idx);
      }
      ++idx;
    }
    return cache;
  }
}
