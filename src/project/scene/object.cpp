/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "object.h"

#include "scene.h"
#include <algorithm>
#include "../../utils/hash.h"
#include "../../utils/jsonBuilder.h"
#include "../../utils/json.h"

using Builder = Utils::JSON::Builder;

namespace
{
  Builder serializeObj(Project::Object &obj)
  {
    Builder builder{};
    builder.set("id", obj.id);
    builder.set("name", obj.name);
    builder.set("uuid", obj.uuid);

    builder.set("selectable", obj.selectable);
    builder.set("enabled", obj.enabled);
    builder.set("isGroup", obj.isGroup);

    builder
      .set(obj.uuidPrefab)
      .set(obj.pos)
      .set(obj.rot)
      .set(obj.scale);

    std::vector<Builder> comps{};
    for (auto &comp : obj.components) {
      auto &def = Project::Component::TABLE[comp.id];
      comps.push_back({});
      Builder &builderCom = comps.back();
      builderCom.set("id", comp.id);
      builderCom.set("uuid", comp.uuid);
      builderCom.set("name", comp.name);
      builderCom.setRaw("data", def.funcSerialize(comp));
    }
    builder.set("components", comps);

    std::vector<Builder> children{};
    for (const auto &child : obj.children) {
      children.push_back(serializeObj(*child));
    }
    builder.set("children", children);
    return builder;
  }
}

void Project::Object::addComponent(int compID) {
  if (compID < 0 || compID >= static_cast<int>(Component::TABLE.size()))return;
  auto &def = Component::TABLE[compID];

  components.push_back({
    .id = compID,
    .uuid = Utils::Hash::sha256_64bit(
      std::to_string(rand()) + std::to_string(compID)
    ),
    .name = std::string{def.name},
    .data = def.funcInit(*this)
  });
}

void Project::Object::removeComponent(uint64_t uuid) {
  std::erase_if(
    components,
    [uuid](const Component::Entry &entry) {
      return entry.uuid == uuid;
    }
  );
}

std::string Project::Object::serialize() {
  return serializeObj(*this).toString();
}

void Project::Object::deserialize(Scene *scene, const simdjson::simdjson_result<simdjson::dom::element>&doc)
{
  if(!doc.is_object())return;

  id = Utils::JSON::readInt(doc, "id");
  name = Utils::JSON::readString(doc, "name");
  uuid = Utils::JSON::readU64(doc, "uuid");

  selectable = Utils::JSON::readBool(doc, "selectable", true);
  enabled = Utils::JSON::readBool(doc, "enabled", true);
  isGroup = Utils::JSON::readBool(doc, "isGroup", false);

  Utils::JSON::readProp(doc, uuidPrefab);
  Utils::JSON::readProp(doc, pos);
  Utils::JSON::readProp(doc, rot);
  Utils::JSON::readProp(doc, scale, {1,1,1});

  auto cmArray = doc["components"].get_array();
  if (cmArray.error() == simdjson::SUCCESS) {
    int count = cmArray.size();
    for (int i=0; i<count; ++i) {
      auto compObj = cmArray.at(i);
      if (compObj.error() != simdjson::SUCCESS)continue;

      auto id = Utils::JSON::readInt(compObj, "id");
      if (id < 0 || id >= static_cast<int>(Component::TABLE.size()))continue;
      auto &def = Component::TABLE[id];

      auto data = compObj["data"].get_object();

      components.push_back({
        .id = id,
        .uuid = Utils::JSON::readU64(compObj, "uuid"),
        .name = Utils::JSON::readString(compObj, "name"),
        .data = def.funcDeserialize(data)
      });

    }
  }

  auto ch = doc["children"];
  if (ch.error() != simdjson::SUCCESS)return;
  auto chArray = ch.get_array();
  if (chArray.error() != simdjson::SUCCESS)return;

  size_t childCount = chArray.size();

  assert(scene || childCount == 0);
  if(!scene)return;

  for (size_t i=0; i<childCount; ++i) {
    auto childObj = std::make_shared<Object>(*this);
    childObj->deserialize(scene, chArray.at(i));
    scene->addObject(*this, childObj);
  }
}
