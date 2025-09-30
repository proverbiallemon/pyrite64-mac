/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "./sceneManager.h"
#include "../../context.h"
#include <filesystem>

namespace
{
  std::string getScenePath() {
    auto scenesPath = ctx.project->getPath() + "/data/scenes";
    if (!std::filesystem::exists(scenesPath)) {
      std::filesystem::create_directory(scenesPath);
    }
    return scenesPath;
  }
}

void Project::SceneManager::reload()
{
  entries.clear();
  if (!ctx.project)return;

  auto scenesPath = getScenePath();

  // list directories
  for (const auto &entry : std::filesystem::directory_iterator{scenesPath}) {
    if (entry.is_directory()) {
      auto path = entry.path();
      auto name = path.filename().string();
      try {
        int id = std::stoi(name);
        entries.push_back({id, name});
      } catch(...) {
        // ignore
      }
    }
  }

}

Project::SceneManager::SceneManager()
{
  reload();
}

void Project::SceneManager::save() {
}

void Project::SceneManager::add() {
  auto scenesPath = getScenePath();
  int newId = 1;
  for (const auto &entry : entries) {
    if (entry.id >= newId) {
      newId = entry.id + 1;
    }
  }
  auto newPath = std::filesystem::path{scenesPath} / std::to_string(newId);
  printf("Create-Scene: %s\n", newPath.c_str());
  std::filesystem::create_directory(newPath);

  reload();
}
