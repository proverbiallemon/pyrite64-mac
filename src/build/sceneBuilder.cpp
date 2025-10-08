/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "projectBuilder.h"
#include "../utils/string.h"
#include <filesystem>

#include "../utils/binaryFile.h"
#include "../utils/fs.h"
#include "../utils/logger.h"

namespace fs = std::filesystem;

namespace
{
  constexpr uint32_t FLAG_CLR_DEPTH = 1 << 0;
  constexpr uint32_t FLAG_CLR_COLOR = 1 << 1;
  constexpr uint32_t FLAG_SCR_32BIT = 1 << 2;
}

void Build::buildScene(Project::Project &project, const Project::SceneEntry &scene, SceneCtx &ctx)
{
  std::string fileNameScene = "s" + Utils::padLeft(std::to_string(scene.id), '0', 4);
  std::string fileNameStr = fileNameScene + "s";
  std::string fileNameObj = fileNameScene + "o";

  std::unique_ptr<Project::Scene> sc{new Project::Scene(scene.id, project.getPath())};

  auto fsDataPath = fs::absolute(fs::path{project.getPath()} / "filesystem" / "p64");

  uint32_t flags = 0;
  uint32_t objCount = sc->objectsMap.size();

  if (sc->conf.doClearDepth)flags |= FLAG_CLR_DEPTH;
  if (sc->conf.doClearColor)flags |= FLAG_CLR_COLOR;
  if (sc->conf.fbFormat)flags |= FLAG_SCR_32BIT;

  ctx.fileObj = {};
  for (auto objEntry : sc->objectsMap) {
    auto &obj = *objEntry.second;
    ctx.fileObj.write<uint16_t>(0); // @TODO type
    ctx.fileObj.write<uint16_t>(obj.id);
    auto posSize = ctx.fileObj.getPos();
    ctx.fileObj.write<uint16_t>(0); // size, filled later

    // DATA
    for (auto &comp : obj.components) {
      ctx.fileObj.write<uint16_t>(comp.id);
      if (comp.id >= 0 && comp.id < Project::Component::TABLE.size()) {
        Project::Component::TABLE[comp.id].funcBuild(obj, comp, ctx);
      } else {
        Utils::Logger::log("Component ID not found: " + std::to_string(comp.id), Utils::Logger::LEVEL_ERROR);
        assert(false);
      }
    }

    auto size = ctx.fileObj.getPos() - posSize + 4;

    ctx.fileObj.posPush(posSize);
      ctx.fileObj.write<uint16_t>(size);
    ctx.fileObj.posPop();

    ctx.fileObj.align(4);
  }

  ctx.fileObj.writeToFile(fsDataPath / fileNameObj);

  ctx.fileScene = {};
  ctx.fileScene.write<uint16_t>(sc->conf.fbWidth);
  ctx.fileScene.write<uint16_t>(sc->conf.fbHeight);
  ctx.fileScene.write(flags);
  ctx.fileScene.writeRGBA(sc->conf.clearColor);
  ctx.fileScene.write(objCount);

  ctx.fileScene.writeToFile(fsDataPath / fileNameScene);

  Utils::FS::saveTextFile(fsDataPath / fileNameStr, "TODO");

  ctx.files.push_back("filesystem/p64/" + fileNameScene);
  ctx.files.push_back("filesystem/p64/" + fileNameStr);
  ctx.files.push_back("filesystem/p64/" + fileNameObj);
}
