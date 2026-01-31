/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "project.h"

#include <filesystem>

#include "../utils/fs.h"

#include "../utils/json.h"
#include "../utils/jsonBuilder.h"

std::string Project::ProjectConf::serialize() const {
  Utils::JSON::Builder builder{};
  builder.set("name", name);
  builder.set("romName", romName);
  builder.set("pathEmu", pathEmu);
  builder.set("pathN64Inst", pathN64Inst);
  builder.set("sceneIdOnBoot", sceneIdOnBoot);
  builder.set("sceneIdOnReset", sceneIdOnReset);
  return builder.toString();
}

void Project::Project::deserialize(const nlohmann::json &doc) {
  conf.name = doc.value("name", "New Project");
  conf.romName = doc.value("romName", "pyrite64");
  conf.pathEmu = doc.value("pathEmu", "ares");
  conf.pathN64Inst = doc.value("pathN64Inst", "");
  conf.sceneIdOnBoot = doc.value("sceneIdOnBoot", 1);
  conf.sceneIdOnReset = doc.value("sceneIdOnReset", 1);
}

Project::Project::Project(const std::string &path)
  : path{path}, pathConfig{path + "/project.json"}
{
  auto configJSON = Utils::JSON::loadFile(pathConfig);
  if (configJSON.empty()) {
    throw std::runtime_error("Not a valid project!");
  }

  Utils::FS::ensureDir(path);
  Utils::FS::ensureDir(path + "/data");
  Utils::FS::ensureDir(path + "/data/scenes");
  Utils::FS::ensureDir(path + "/assets");
  Utils::FS::ensureDir(path + "/assets/p64");
  Utils::FS::ensureDir(path + "/src");
  Utils::FS::ensureDir(path + "/src/p64");
  Utils::FS::ensureDir(path + "/src/user");

  Utils::FS::ensureFile(path + "/.gitignore", "data/build/baseGitignore");
  Utils::FS::ensureFile(path + "/Makefile.custom", "data/build/baseMakefile.custom");
  Utils::FS::ensureFile(path + "/assets/p64/font.ia4.png", "data/build/assets/font.ia4.png");

  deserialize(configJSON);
  assets.reload();
  scenes.reload();
}

void Project::Project::save() {
  Utils::FS::saveTextFile(pathConfig, conf.serialize());
  assets.save();
  scenes.save();
}
