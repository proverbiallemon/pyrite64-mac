/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "SHA256.h"

namespace Utils::Hash
{
  inline uint64_t sha256_64bit(const std::string& str)
  {
    SHA256 sha{};
    sha.update(str);
    auto hash = sha.digest();
    uint64_t res = 0;
    for (int i = 0; i < 8; i++) {
      res = (res << 8) | hash[i];
    }
    return res;
  }

  inline uint32_t sha256_32bit(const std::string& str) {
    return static_cast<uint32_t>(sha256_64bit(str) & 0xFFFFFFFF);
  }

  constexpr uint64_t crc64(const std::string_view& str)
  {
    uint64_t crc = 0xFFFFFFFFFFFFFFFF;
    for (char c : str) {
      crc ^= static_cast<uint8_t>(c);
      for (int i = 0; i < 8; i++) {
        if (crc & 1) {
          crc = (crc >> 1) ^ 0xC96C5795D7870F42;
        } else {
          crc >>= 1;
        }
      }
    }
    return crc;
  }

  uint64_t randomU64();

  inline uint32_t randomU32() {
    return randomU64() & 0xFFFFFFFF;
  }

}
