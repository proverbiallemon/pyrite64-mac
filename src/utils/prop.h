/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <cstdint>
#include <utility>

#include "hash.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtc/quaternion.hpp"

template<typename T>
struct Property
{
  std::string name{};
  uint64_t id{};
  bool override{false};
  T value{};

  constexpr Property() = default;

  constexpr explicit Property(const char* const propName)
    : name{propName}, id{Utils::Hash::crc64(name)}
  {}

  constexpr explicit Property(std::string propName, T val)
    : name{std::move(propName)}, id{Utils::Hash::crc64(name)}, value{val}
  {}

  const T& resolve() const
  {
    // @TODO: implement hierarchy lookup
    return value;
  }
};

using PropU32 = Property<uint32_t>;
using PropS32 = Property<int32_t>;
using PropU64 = Property<uint64_t>;
using PropS64 = Property<int64_t>;

using PropFloat = Property<float>;
using PropBool = Property<bool>;

using PropVec3 = Property<glm::vec3>;
using PropVec4 = Property<glm::vec4>;
using PropQuat = Property<glm::quat>;

using PropString = Property<std::string>;

#define PROP_U32(name) Property<uint32_t> name{#name}
#define PROP_S32(name) Property<int32_t> name{#name}
#define PROP_U64(name) Property<uint64_t> name{#name}
#define PROP_S64(name) Property<int64_t> name{#name}
#define PROP_FLOAT(name) Property<float> name{#name}
#define PROP_BOOL(name) Property<bool> name{#name}
#define PROP_VEC3(name) Property<glm::vec3> name{#name}
#define PROP_VEC4(name) Property<glm::vec4> name{#name}
#define PROP_QUAT(name) Property<glm::quat> name{#name}
#define PROP_STRING(name) Property<std::string> name{#name}