/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "shader.h"

#include <cassert>
#include <stdexcept>

#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_iostream.h"

Renderer::Shader::Shader(SDL_GPUDevice* device, const Config &conf)
  : gpuDevice{device}
{

  SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
	SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;

  std::string pathVert = "data/shader/" + conf.name + ".vert.";
  std::string pathFrag = "data/shader/" + conf.name + ".frag.";
  const char* entrypoint{};

  if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
    pathVert += "spv";
    pathFrag += "spv";
		format = SDL_GPU_SHADERFORMAT_SPIRV;
		entrypoint = "main";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
		pathVert += "msl";
		pathFrag += "msl";
		format = SDL_GPU_SHADERFORMAT_MSL;
		entrypoint = "main0";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
		pathVert += "dxil";
    pathFrag += "dxil";
		format = SDL_GPU_SHADERFORMAT_DXIL;
		entrypoint = "main";
	} else {
    throw std::runtime_error("Unknown Shader-Backend!");
	}

  size_t vertexCodeSize;
  void* vertexCode = SDL_LoadFile(pathVert.c_str(), &vertexCodeSize);
  assert(vertexCode != nullptr);

  size_t fragmentCodeSize;
  void* fragmentCode = SDL_LoadFile(pathFrag.c_str(), &fragmentCodeSize);
  assert(vertexCode != nullptr);

  SDL_GPUShaderCreateInfo vertexInfo{};
  vertexInfo.code = (Uint8*)vertexCode;
  vertexInfo.code_size = vertexCodeSize;
  vertexInfo.entrypoint = entrypoint;
  vertexInfo.format = format;
  vertexInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
  vertexInfo.num_samplers = conf.vertTexCount;
  vertexInfo.num_storage_buffers = 0;
  vertexInfo.num_storage_textures = 0;
  vertexInfo.num_uniform_buffers = conf.vertUboCount;
  shaderVert = SDL_CreateGPUShader(device, &vertexInfo);
  assert(shaderVert != nullptr);

  // create the fragment shader
  SDL_GPUShaderCreateInfo fragmentInfo{};
  fragmentInfo.code = (Uint8*)fragmentCode;
  fragmentInfo.code_size = fragmentCodeSize;
  fragmentInfo.entrypoint = entrypoint;
  fragmentInfo.format = format;
  fragmentInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
  fragmentInfo.num_samplers = conf.fragTexCount;
  fragmentInfo.num_storage_buffers = 0;
  fragmentInfo.num_storage_textures = 0;
  fragmentInfo.num_uniform_buffers = conf.fragUboCount;

  shaderFrag = SDL_CreateGPUShader(gpuDevice, &fragmentInfo);

  SDL_free(fragmentCode);
  SDL_free(vertexCode);
}

Renderer::Shader::~Shader()
{
  SDL_ReleaseGPUShader(gpuDevice, shaderVert);
  SDL_ReleaseGPUShader(gpuDevice, shaderFrag);
}
