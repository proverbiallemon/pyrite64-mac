/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "sceneContext.h"
#include "../project/project.h"

namespace Build
{
  // helper
  bool assetBuildNeeded(const Project::AssetManagerEntry &asset, const std::string &outPath);

  // Asset builds
  void buildScene(Project::Project &project, const Project::SceneEntry &scene, SceneCtx &ctx);
  void buildScripts(Project::Project &project, SceneCtx &sceneCtx);
  void buildGlobalScripts(Project::Project &project, SceneCtx &sceneCtx);

  bool buildT3DMAssets(Project::Project &project, SceneCtx &sceneCtx);
  bool buildFontAssets(Project::Project &project, SceneCtx &sceneCtx);
  bool buildTextureAssets(Project::Project &project, SceneCtx &sceneCtx);
  bool buildAudioAssets(Project::Project &project, SceneCtx &sceneCtx);
  bool buildPrefabAssets(Project::Project &project, SceneCtx &sceneCtx);

  bool buildProject(const std::string &path);

  // individual parts
  void writeObject(SceneCtx &ctx, Project::Object &obj, bool savePrefabItself = false);

  bool buildT3DCollision(
    Project::Project &project, SceneCtx &sceneCtx,
    const std::unordered_set<std::string> &meshes,
    uint64_t orgUUID,
    uint64_t newUUID
  );

  Utils::BinaryFile buildCollision(const std::string &gltfPath, float baseScale, const std::unordered_set<std::string> &meshes = {});
}
