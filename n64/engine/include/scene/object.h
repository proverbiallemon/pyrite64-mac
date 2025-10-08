/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace P64
{
  class Object
  {
    private:

    public:
      uint16_t id{};
      // @TODO: avoid vector
      std::vector<uint32_t> compRefs{};
  };
}