/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "renderer/bigtex/textures.h"
#include "bigtex.h"

namespace {
  constexpr uint32_t TEX_WIDTH = 256;
  constexpr uint32_t TEX_HEIGHT = 256;
  constexpr uint32_t TEX_SIZE_BYTES = TEX_WIDTH * TEX_HEIGHT;

  std::string getName(const std::string &path)
  {
    auto pos = path.find_last_of('/');
    if(pos != std::string::npos) {
      pos += 1;
    } else {
      pos = 0;
    }
    return path.substr(pos);
  }

  void loadIntoBuffer(const char* path, uint8_t *buffer)
  {
    auto *fp = asset_fopen(path, nullptr);
    fread(buffer, 1, TEX_SIZE_BYTES, fp);
    fclose(fp);
  }
}

namespace P64::Renderer::BigTex
{
  Textures::Textures(uint32_t maxSize_)
    : maxSize{maxSize_}, idx{0}
  {
    uint32_t allocSize = TEX_SIZE_BYTES * maxSize;
    debugf("Reserve %.2fKB (%.2fMB) for %lu textures\n", (double)allocSize/1024.0, (double)allocSize/1024.0/1024.0, maxSize);
    buffer = (uint8_t*)TEX_BASE_ADDR;
    //buffer = (uint16_t*)malloc_uncached(allocSize);
  }

  Textures::~Textures() {
    //free_uncached(buffer);
  }

  uint8_t Textures::addTexture(const std::string &path)
  {
    auto name = getName(path);
    auto it = texMap.find(name);
    if(it == texMap.end()) {
      auto ptrOut = buffer + TEX_SIZE_BYTES * idx;
      loadIntoBuffer(path.c_str(), ptrOut);

      auto newIdx = idx++;
      texMap[name] = newIdx;
      assertf(idx < maxSize, "Texture buffer full: %lu/%lu", idx, maxSize);
      return newIdx;
    }
    return it->second;
  }

  uint8_t Textures::reserveTexture() {
    auto newIdx = idx++;
    assertf(idx < maxSize, "Texture buffer full: %lu/%lu", idx, maxSize);
    return newIdx;
  }

  uint8_t Textures::setTexture(uint8_t texIdx, const std::string &pathNew)
  {
    auto ptrOut = buffer + TEX_SIZE_BYTES * texIdx;
    loadIntoBuffer(pathNew.c_str(), ptrOut);
    return texIdx;
  }
}