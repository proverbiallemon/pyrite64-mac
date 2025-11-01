/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "../components.h"
#include "../../../context.h"
#include "../../../editor/imgui/helper.h"
#include "../../../utils/json.h"
#include "../../../utils/jsonBuilder.h"
#include "../../../utils/binaryFile.h"
#include "../../../utils/logger.h"
#include "../../assetManager.h"
#include "../../../editor/pages/parts/viewport3D.h"
#include "../../../renderer/scene.h"
#include "../../../utils/meshGen.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"

namespace
{
  /*constexpr char* const LIGHT_TYPES[LIGHT_TYPE_COUNT] = {
    "Ambient",
    "Directional",
    "Point"
  };*/

  glm::vec3 rotToDir(const Project::Object &obj) {
    return glm::normalize(obj.rot * glm::vec3{0,0,-1});
  }
}

namespace Project::Component::Camera
{
  struct Data
  {
    glm::ivec2 vpOffset{0,0};
    glm::ivec2 vpSize{0,0};
    float fov{glm::radians(75.0f)};
    float near{0.1f};
    float far{100.0f};
  };

  std::shared_ptr<void> init(Object &obj) {
    auto data = std::make_shared<Data>();
    return data;
  }

  std::string serialize(Entry &entry) {
    Data &data = *static_cast<Data*>(entry.data.get());
    Utils::JSON::Builder builder{};

    builder.set("vpOffset", data.vpOffset);
    builder.set("vpSize", data.vpSize);
    builder.set("fov", data.fov);
    builder.set("near", data.near);
    builder.set("far", data.far);
    return builder.toString();
  }

  std::shared_ptr<void> deserialize(simdjson::simdjson_result<simdjson::dom::object> &doc) {
    auto data = std::make_shared<Data>();
    data->vpOffset = Utils::JSON::readVec2(doc, "vpOffset");;
    data->vpSize = Utils::JSON::readVec2(doc, "vpSize");;
    data->fov = Utils::JSON::readFloat(doc, "fov");
    data->near = Utils::JSON::readFloat(doc, "near");
    data->far = Utils::JSON::readFloat(doc, "far");
    return data;
  }

  void build(Object& obj, Entry &entry, Build::SceneCtx &ctx)
  {
    Data &data = *static_cast<Data*>(entry.data.get());
    ctx.fileObj.writeArray(glm::value_ptr(data.vpOffset), 2);
    ctx.fileObj.writeArray(glm::value_ptr(data.vpSize), 2);
    ctx.fileObj.write<float>(data.fov);
    ctx.fileObj.write<float>(data.near);
    ctx.fileObj.write<float>(data.far);
  }

  void update(Object &obj, Entry &entry)
  {
    Data &data = *static_cast<Data*>(entry.data.get());
  }

  void draw(Object &obj, Entry &entry) {
    Data &data = *static_cast<Data*>(entry.data.get());

    if (ImGui::InpTable::start("Comp"))
    {
      auto scene = ctx.project->getScenes().getLoadedScene();
      assert(scene);
      if(data.vpSize.x == 0) data.vpSize.x = scene->conf.fbWidth;
      if(data.vpSize.y == 0) data.vpSize.y = scene->conf.fbHeight;

      ImGui::InpTable::addString("Name", entry.name);
      ImGui::InpTable::add("Offset");
      ImGui::InputInt2("##vpOffset", &data.vpOffset.x);
      ImGui::InpTable::add("Size");
      ImGui::InputInt2("##vpSize", &data.vpSize.x);

      float fov = glm::degrees(data.fov);
      ImGui::InpTable::addInputFloat("FOV", fov);
      data.fov = glm::radians(fov);


      ImGui::InpTable::addInputFloat("Near", data.near);
      ImGui::InpTable::addInputFloat("Far", data.far);
      //ImGui::InpTable::addComboBox("Type", data.type, LIGHT_TYPES, LIGHT_TYPE_COUNT);
      ImGui::InpTable::end();
    }
  }

  void draw3D(Object& obj, Entry &entry, Editor::Viewport3D &vp, SDL_GPUCommandBuffer* cmdBuff, SDL_GPURenderPass* pass)
  {
    Data &data = *static_cast<Data*>(entry.data.get());

    constexpr float BOX_SIZE = 0.125f;
    constexpr float LINE_LEN = 0.7f;
    glm::u8vec4 col{0xFF};

    bool isSelected = ctx.selObjectUUID == obj.uuid;
/*
    if(isSelected)
    {
      Utils::Mesh::addLineBox(*vp.getLines(), obj.pos, {BOX_SIZE, BOX_SIZE, BOX_SIZE}, col);
      if(data.type == LIGHT_TYPE_DIRECTIONAL)
      {
        glm::vec3 dir = rotToDir(obj);
        Utils::Mesh::addLine(*vp.getLines(), obj.pos, obj.pos + (dir * -LINE_LEN), col);
      }
    }
*/
    Utils::Mesh::addSprite(*vp.getSprites(), obj.pos, obj.uuid, 3, col);
  }
}
