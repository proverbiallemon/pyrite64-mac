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

namespace Project::Component::Code
{
  struct Data
  {
    uint64_t scriptUUID{0};
  };

  std::shared_ptr<void> init(Object &obj) {
    auto data = std::make_shared<Data>();
    return data;
  }

  std::string serialize(Entry &entry) {
    Data &data = *static_cast<Data*>(entry.data.get());
    Utils::JSON::Builder builder{};
    builder.set("script", data.scriptUUID);
    return builder.toString();
  }

  std::shared_ptr<void> deserialize(simdjson::simdjson_result<simdjson::dom::object> &doc) {
    auto data = std::make_shared<Data>();
    data->scriptUUID = doc["script"].get<uint64_t>();
    return data;
  }

  void build(Object&, Entry &entry, Build::SceneCtx &ctx)
  {
    Data &data = *static_cast<Data*>(entry.data.get());

    auto idRes = ctx.codeIdxMapUUID.find(data.scriptUUID);
    uint16_t id = 0xDEAD;
    if (idRes == ctx.codeIdxMapUUID.end()) {
      Utils::Logger::log("Component Code: Script UUID not found: " + std::to_string(entry.uuid), Utils::Logger::LEVEL_ERROR);
    } else {
      id = idRes->second;
    }

    ctx.fileObj.write<uint16_t>(id);
  }

  const char* getter(void* user_data, int idx)
  {
    auto &scriptList = ctx.project->getAssets().getScriptEntries();
    if (idx < 0 || idx >= scriptList.size())return "<Select Script>";
    return scriptList[idx].name.c_str();
  }

  void draw(Object &obj, Entry &entry)
  {
    Data &data = *static_cast<Data*>(entry.data.get());

    auto &assets = ctx.project->getAssets();
    auto &scriptList = assets.getScriptEntries();

    if (ImGui::InpTable::start("Comp")) {
      ImGui::InpTable::addString("Name", entry.name);
      ImGui::InpTable::add("Script");
      //ImGui::InputScalar("##UUID", ImGuiDataType_U64, &data.scriptUUID);

      int idx = scriptList.size();
      auto res = assets.entriesMapScript.find(data.scriptUUID);
      if (res != assets.entriesMapScript.end()) {
        idx = res->second;
      }

      ImGui::Combo("##UUID", &idx, getter, nullptr, scriptList.size()+1);
      data.scriptUUID = scriptList[idx].uuid;

      ImGui::InpTable::end();
    }
  }
}
