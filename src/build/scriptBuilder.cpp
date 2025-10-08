/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "projectBuilder.h"
#include "../utils/string.h"
#include <filesystem>
#include <format>

#include "../utils/fs.h"
#include "../utils/logger.h"

namespace fs = std::filesystem;

void Build::buildScripts(Project::Project &project, SceneCtx &sceneCtx)
{
  auto pathTable = project.getPath() + "/src/p64/scriptTable.cpp";

  std::string srcEntries = "";
  std::string srcDecl = "";

  auto scripts = project.getAssets().getScriptEntries();
  uint32_t idx = 0;
  for (auto &script : scripts)
  {
    auto uuidStr = std::format("{:016X}", script.uuid);

    srcEntries += "    " + uuidStr + "::update,\n";
    srcDecl += "  namespace " + uuidStr + " { void update(); }\n";

    sceneCtx.codeIdxMapUUID[script.uuid] = idx;

    Utils::Logger::log("Script: " + uuidStr + " -> " + std::to_string(idx));
    ++idx;
  }

  auto src = Utils::FS::loadTextFile("data/scripts/scriptTable.cpp");
  src = Utils::replaceAll(src, "__CODE_ENTRIES__", srcEntries);
  src = Utils::replaceAll(src, "__CODE_DECL__", srcDecl);


  Utils::FS::saveTextFile(pathTable, src);
}