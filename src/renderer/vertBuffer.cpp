/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "vertBuffer.h"

Renderer::VertBuffer::VertBuffer(size_t maxCount, SDL_GPUDevice* device)
  : gpuDevice{device}
{
  auto size = maxCount * sizeof(Vertex);

  SDL_GPUBufferCreateInfo bufferInfo{};
  bufferInfo.size = size;
  bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  buffer = SDL_CreateGPUBuffer(gpuDevice, &bufferInfo);

  SDL_GPUTransferBufferCreateInfo transferInfo{};
  transferInfo.size = sizeof(size);
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  bufferTrans = SDL_CreateGPUTransferBuffer(gpuDevice, &transferInfo);
}

Renderer::VertBuffer::~VertBuffer()
{
  SDL_ReleaseGPUBuffer(gpuDevice, buffer);
  SDL_ReleaseGPUTransferBuffer(gpuDevice, bufferTrans);
}

void Renderer::VertBuffer::setData(const std::vector<Vertex>&vertices) {
  currByteSize = vertices.size() * sizeof(Vertex);
  auto data = (Vertex*)SDL_MapGPUTransferBuffer(gpuDevice, bufferTrans, false);
  SDL_memcpy(data, vertices.data(), currByteSize);
  SDL_UnmapGPUTransferBuffer(gpuDevice, bufferTrans);
  needsUpload = true;
}

void Renderer::VertBuffer::upload(SDL_GPUCopyPass &pass) {
  if (!needsUpload || currByteSize == 0)return;

  SDL_GPUTransferBufferLocation location{};
  location.transfer_buffer = bufferTrans;
  location.offset = 0;

  SDL_GPUBufferRegion region{};
  region.buffer = buffer;
  region.size = currByteSize;
  region.offset = 0;

  SDL_UploadToGPUBuffer(&pass, &location, &region, true);
  needsUpload = false;
}
