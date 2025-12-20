/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "prefab.h"

#include "../../utils/json.h"
#include "../../utils/jsonBuilder.h"

using Builder = Utils::JSON::Builder;

std::string Project::Prefab::serialize()
{
  Builder builder{};
  builder.set(uuid);
  builder.setRaw("obj", obj.serialize());
  return builder.toString();
}

void Project::Prefab::deserialize(const simdjson::simdjson_result<simdjson::dom::element> &doc)
{
  if(!doc.is_object())return;
  Utils::JSON::readProp(doc, uuid);
  obj.deserialize(nullptr, doc);
}
