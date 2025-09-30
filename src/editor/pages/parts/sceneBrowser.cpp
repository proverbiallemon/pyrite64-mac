/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "sceneBrowser.h"

#include "imgui.h"
#include "../../../context.h"

Editor::SceneBrowser::SceneBrowser() {
}

Editor::SceneBrowser::~SceneBrowser() {
}

void Editor::SceneBrowser::draw()
{
  if (ImGui::Button("Add Scene")) {
    ctx.project->getScenes().add();
  }

  auto &scenes = ctx.project->getScenes().getEntries();
  for (auto &scene : scenes) {
    ImGui::Bullet();
    ImGui::SameLine();
    ImGui::Text("%d - %s", scene.id, scene.name.c_str());
  }
}
