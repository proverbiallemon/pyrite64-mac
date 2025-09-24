/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <string>
#include <vector>

#include "vertex.h"
#include "SDL3/SDL_gpu.h"

namespace Renderer
{
  class VertBuffer
  {
    private:
      SDL_GPUDevice* gpuDevice{nullptr};
      SDL_GPUBuffer* buffer{nullptr};
      SDL_GPUTransferBuffer* bufferTrans{nullptr};
      size_t currByteSize{0};
      bool needsUpload{false};

    public:
      VertBuffer(size_t maxCount, SDL_GPUDevice* device);
      ~VertBuffer();

      void setData(const std::vector<Vertex> &vertices);

      void upload(SDL_GPUCopyPass& pass);

      void addBinding(SDL_GPUBufferBinding &binding) const {
        binding.buffer = buffer;
        binding.offset = 0;
      }
  };
}