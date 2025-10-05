/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "fs.h"
#include "simdjson.h"
#include "SDL3/SDL_iostream.h"

namespace Utils::JSON
{
  inline simdjson::dom::parser parser{};

  inline simdjson::simdjson_result<simdjson::dom::element> load(const std::string &json) {
    auto doc = parser.parse(std::string_view{json});
    return doc;
  }

  inline simdjson::simdjson_result<simdjson::dom::element> loadFile(const std::string &path) {
    auto jsonData = FS::loadTextFile(path);

    if (jsonData.empty()) {
      return {};
    }

    auto doc = parser.parse(std::string_view{jsonData});
    return doc;
  }

  inline std::string readString(const simdjson::simdjson_result<simdjson::dom::element> &el, const std::string &key) {
    auto val = el[key];
    if (val.error() != simdjson::SUCCESS) {
      return "";
    }
    auto str = val.get_string();
    if (str.error() != simdjson::SUCCESS) {
      return "";
    }
    return std::string{str->data(), str->length()};
  }

  inline int readInt(const simdjson::simdjson_result<simdjson::dom::element> &el, const std::string &key) {
    auto val = el[key];
    if (val.error() != simdjson::SUCCESS) {
      return 0;
    }
    auto i = val.get_int64();
    if (i.error() != simdjson::SUCCESS) {
      return 0;
    }
    return (int)(*i);
  }

  inline float readFloat(const simdjson::simdjson_result<simdjson::dom::element> &el, const std::string &key) {
    auto val = el[key];
    if (val.error() != simdjson::SUCCESS) {
      return 0.0f;
    }
    auto f = val.get_double();
    if (f.error() != simdjson::SUCCESS) {
      return 0.0f;
    }
    return (float)(*f);
  }

  inline bool readBool(const simdjson::simdjson_result<simdjson::dom::element> &el, const std::string &key) {
    auto val = el[key];
    if (val.error() != simdjson::SUCCESS) {
      return false;
    }
    auto b = val.get_bool();
    if (b.error() != simdjson::SUCCESS) {
      return false;
    }
    return *b;
  }
}

#define JSON_GET_STR(key) conf.key = Utils::JSON::readString(doc, #key);
#define JSON_GET_INT(key) conf.key = (typeof(conf.key))Utils::JSON::readInt(doc, #key);
#define JSON_GET_FLOAT(key) conf.key = Utils::JSON::readFloat(doc, #key);
#define JSON_GET_BOOL(key) conf.key = Utils::JSON::readBool(doc, #key);

#define JSON_SET_STR_LAST(key) \
builder.append_key_value<#key>(conf.key);

#define JSON_SET_STR(key) \
  JSON_SET_STR_LAST(key) \
  builder.append_comma();

