/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <vector>

#include "stringTable.h"
#include "../utils/binaryFile.h"

namespace Project { class Project; class Scene; struct AssetManagerEntry; }

namespace Build
{
  struct AssetEntry
  {
    std::string path{};
    uint32_t stringOffset{};
    uint32_t type{};
  };

  struct SceneCtx
  {
    Project::Project *project{};
    Project::Scene *scene{};
    std::vector<std::string> files{};
    std::array<uint64_t, 16> autoLoadFontUUIDs{};
    std::unordered_map<uint64_t, uint32_t> codeIdxMapUUID{};
    Utils::BinaryFile fileScene{};
    Utils::BinaryFile fileObj{};
    StringTable strTable{};

    std::vector<AssetEntry> assetList{};
    std::unordered_map<uint64_t, uint32_t> assetUUIDToIdx{};
    std::string assetFileMap{};
    uint32_t stringOffset{0};

    void addAsset(const Project::AssetManagerEntry &entry);
  };
}
