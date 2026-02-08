/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "projectBuilder.h"
#include "../utils/string.h"
#include "../utils/fs.h"
#include "../utils/logger.h"
#include "../utils/proc.h"
#include <filesystem>

namespace fs = std::filesystem;

bool Build::buildPrefabAssets(Project::Project &project, SceneCtx &sceneCtx)
{
  fs::path mkAsset = fs::path{project.conf.pathN64Inst} / "bin" / "mkasset";
  auto &assets = sceneCtx.project->getAssets().getTypeEntries(Project::FileType::PREFAB);
  for (auto &asset : assets)
  {
    if(asset.conf.exclude)continue;

    auto projectPath = fs::path{project.getPath()};
    auto outPath = projectPath / asset.outPath;
    auto outDir = outPath.parent_path();
    Utils::FS::ensureDir(outPath.parent_path());

    sceneCtx.files.push_back(Utils::FS::toUnixPath(asset.outPath));
    if(!assetBuildNeeded(asset, outPath))continue;

    //printf("Prefab: %s -> %s\n", asset.path.c_str(), outPath.string().c_str());

    sceneCtx.fileObj = {};
    writeObject(sceneCtx, asset.prefab->obj, true);
    sceneCtx.fileObj.writeToFile(outPath);
    sceneCtx.fileObj = {};
  }
  return true;
}