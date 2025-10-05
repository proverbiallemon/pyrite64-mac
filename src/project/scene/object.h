/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <memory>
#include <utility>
#include <vector>

namespace Project
{
  class Object
  {
    public:
      Object* parent;

      std::string name{};
      uint64_t uuid{0};
      uint16_t id{};

      std::vector<std::shared_ptr<Object>> children{};

      explicit Object(Object* parent) : parent{parent} {}

      std::string serialize();
  };
}
