/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "projectBuilder.h"
#include "../utils/string.h"
#include "../utils/fs.h"
#include <filesystem>

#include "../project/graph/graph.h"

namespace fs = std::filesystem;

bool Build::buildNodeGraphAssets(Project::Project &project, SceneCtx &sceneCtx)
{
  fs::path mkAsset = fs::path{project.conf.pathN64Inst} / "bin" / "mkasset";
  fs::path sourcePath = fs::path{project.getPath()} / "src" / "p64";
  auto &assets = sceneCtx.project->getAssets().getTypeEntries(Project::FileType::NODE_GRAPH);
  for (auto &asset : assets)
  {
    if(asset.conf.exclude)continue;

    auto projectPath = fs::path{project.getPath()};
    auto outPath = projectPath / asset.outPath;
    auto outDir = outPath.parent_path();
    Utils::FS::ensureDir(outPath.parent_path());

    std::string sourceName = Utils::toHex64(asset.getUUID()) + ".cpp";
    fs::path sourceOutPath = sourcePath / sourceName;

    sceneCtx.files.push_back(Utils::FS::toUnixPath(asset.outPath));
    sceneCtx.graphFunctions.push_back(asset.getUUID());

    if(!assetBuildNeeded(asset, outPath) && std::filesystem::exists(sourceOutPath))continue;

    auto json = Utils::FS::loadTextFile(asset.path);
    Project::Graph::Graph graph{};
    graph.deserialize(json);

    Utils::BinaryFile binFile{};
    std::string sourceCode{};
    sourceCode += "// AUTO-GENERATED FILE\n";
    sourceCode += "// File: " + asset.getName() + "\n\n";

    graph.build(binFile, sourceCode, asset.getUUID());
    binFile.writeToFile(outPath);

    Utils::FS::saveTextFile(sourceOutPath, sourceCode);
  }
  return true;
}