/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "scene.h"
#include "object.h"
#include <SDL3/SDL.h>
#include "IconsFontAwesome4.h"
#include "simdjson.h"
#include "../../utils/json.h"
#include "../../context.h"
#include "../../utils/hash.h"

namespace
{
  std::string getConfPath(int id) {
    auto scenesPath = ctx.project->getPath() + "/data/scenes";
    return scenesPath + "/" + std::to_string(id) + "/scene.json";
  }

  constinit uint64_t nextUUID{1};
}

std::string Project::SceneConf::serialize() const {
  simdjson::builder::string_builder builder{};
  builder.start_object();
  builder.append_key_value<"name">(name);
  builder.append_comma();
  builder.append_key_value<"fbWidth">(fbWidth);
  builder.append_comma();
  builder.append_key_value<"fbHeight">(fbHeight);
  builder.append_comma();
  builder.append_key_value<"fbFormat">(fbFormat);
  builder.append_comma();

  builder.escape_and_append_with_quotes("clearColor");
  builder.append_colon();
  builder.start_array();
    builder.append(clearColor.r); builder.append_comma();
    builder.append(clearColor.g); builder.append_comma();
    builder.append(clearColor.b); builder.append_comma();
    builder.append(clearColor.a);
  builder.end_array();
  builder.append_comma();

  builder.append_key_value<"doClearColor">(doClearColor);
  builder.append_comma();
  builder.append_key_value<"doClearDepth">(doClearDepth);
  builder.end_object();
  return {builder.c_str()};
}

Project::Scene::Scene(int id_)
  : id{id_}
{
  printf("Load scene %d\n", id);

  root.id = 0;
  root.name = "Scene";
  root.uuid = Utils::Hash::sha256_64bit(root.name);
  deserialize(Utils::FS::loadTextFile(getConfPath(id)));
}

std::shared_ptr<Project::Object> Project::Scene::addObject(Object &parent) {
  auto child = std::make_shared<Object>(&parent);
  child->id = nextUUID++;
  child->name = "New Object ("+std::to_string(child->id)+")";
  child->uuid = Utils::Hash::sha256_64bit(child->name + std::to_string(rand()));

  parent.children.push_back(child);
  objectsMap[child->uuid] = child;
  return child;
}

void Project::Scene::removeObject(Object &obj) {
  if (ctx.selObjectUUID == obj.uuid) {
    ctx.selObjectUUID = 0;
  }

  std::erase_if(
    obj.parent->children,
    [&obj](const std::shared_ptr<Object> &ref) { return ref->uuid == obj.uuid; }
  );
  objectsMap.erase(obj.uuid);
}

void Project::Scene::save()
{
  auto pathConfig = getConfPath(id);
  Utils::FS::saveTextFile(pathConfig, serialize());
}

std::string Project::Scene::serialize() {
  auto json = conf.serialize();
  auto graph = root.serialize();

  return std::string{"{\n"}
    + "\"conf\": " + json + ",\n"
    + "\"graph\": " + graph + "\n"
    + "}\n";
}

void Project::Scene::deserialize(const std::string &data)
{
  if(data.empty())return;

  auto doc = Utils::JSON::load(data);
  if (!doc.is_object())return;

  auto docConf = doc["conf"];
  if (docConf.is_object()) {
    conf.name = Utils::JSON::readString(docConf, "name");
    conf.fbWidth = Utils::JSON::readInt(docConf, "fbWidth");
    conf.fbHeight = Utils::JSON::readInt(docConf, "fbHeight");
    conf.fbFormat = Utils::JSON::readInt(docConf, "fbFormat");

    auto clearColor = docConf["clearColor"];
    if (!clearColor.error()) {
      auto col = clearColor.get_array();
      conf.clearColor.r = col.at(0).get_double();
      conf.clearColor.g = col.at(1).get_double();
      conf.clearColor.b = col.at(2).get_double();
      conf.clearColor.a = col.at(3).get_double();
    }
    conf.doClearColor = Utils::JSON::readBool(docConf, "doClearColor");
    conf.doClearDepth = Utils::JSON::readBool(docConf, "doClearDepth");
  }
}
