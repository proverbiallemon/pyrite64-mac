/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "project.h"
#include <optional>
#include "../context.h"
#include "../utils/fs.h"

#include "../utils/json.h"

using namespace simdjson;

std::string Project::ProjectConf::serialize() const {
  builder::string_builder builder{};
  builder.start_object();
  builder.append_key_value<"name">(name);
  builder.append_comma();
  builder.append_key_value<"romName">(romName);
  builder.append_comma();
  builder.append_key_value<"pathEmu">(pathEmu);
  builder.append_comma();
  builder.append_key_value<"pathN64Inst">(pathN64Inst);
  builder.end_object();
  return {builder.c_str()};
}

void Project::Project::deserialize(const simdjson_result<dom::element> &doc) {
  JSON_GET_STR(name);
  JSON_GET_STR(romName);
  JSON_GET_STR(pathEmu);
  JSON_GET_STR(pathN64Inst);
}

Project::Project::Project(const std::string &path)
  : path{path}, pathConfig{path + "/project.json"}
{
  deserialize(Utils::JSON::loadFile(pathConfig));
  assets.reload();
  scenes.reload();
}

void Project::Project::save() {
  Utils::FS::saveTextFile(pathConfig, conf.serialize());
  assets.save();
  scenes.save();
}
