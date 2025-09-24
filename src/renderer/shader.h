/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <string>

#include "SDL3/SDL_gpu.h"

namespace Renderer
{
  class Shader
  {
    private:
      SDL_GPUDevice* gpuDevice{nullptr};
      SDL_GPUShader* shaderVert{nullptr};
      SDL_GPUShader* shaderFrag{nullptr};

    public:
      Shader(const std::string& name, SDL_GPUDevice* device);
      ~Shader();

      void setToPipeline(SDL_GPUGraphicsPipelineCreateInfo &pipelineInfo) const {
        pipelineInfo.vertex_shader = shaderVert;
        pipelineInfo.fragment_shader = shaderFrag;
      }
  };
}
