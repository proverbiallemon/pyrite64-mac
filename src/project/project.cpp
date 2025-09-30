/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "project.h"
#include <optional>
#include "../context.h"

#include "../utils/json.h"

using namespace simdjson;

void Project::Project::deserialize(const simdjson_result<dom::element> &doc) {
  JSON_GET_STR(name);
  JSON_GET_STR(romName);
  JSON_GET_STR(pathEmu);
  JSON_GET_STR(pathLibdragon);
  JSON_GET_STR(pathN64Inst);
}

std::string Project::Project::serialize() const {
  builder::string_builder builder{};
  builder.start_object();
  JSON_SET_STR(name);
  JSON_SET_STR(romName);
  JSON_SET_STR(pathEmu);
  JSON_SET_STR(pathLibdragon);
  JSON_SET_STR_LAST(pathN64Inst);
  builder.end_object();
  return {builder.c_str()};
}

Project::Project::Project(const std::string &path)
  : path{path}, pathConfig{path + "/project.json"}
{
  assert(ctx.project == nullptr);
  ctx.project = this;
  deserialize(Utils::JSON::loadFile(pathConfig));
  assets.reload();
  scenes.reload();
}

void Project::Project::save() {
  auto json = serialize();
  SDL_SaveFile(pathConfig.c_str(), json.c_str(), json.size());

  assets.save();
  scenes.save();
}
