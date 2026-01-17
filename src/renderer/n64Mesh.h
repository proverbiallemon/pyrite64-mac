/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "mesh.h"
#include "texture.h"
#include "uniforms.h"
#include "tiny3d/tools/gltf_importer/src/structs.h"

namespace Project { class AssetManager; }

namespace Renderer
{
  class N64Mesh
  {
    public:
      struct MeshPart
      {
        uint32_t indicesOffset{0};
        uint32_t indicesCount{0};
        UniformN64Material material{};

        SDL_GPUTextureSamplerBinding texBindings[2]{};

        std::weak_ptr<Renderer::Texture> refTex0{};
        std::weak_ptr<Renderer::Texture> refTex1{};
      };
    private:

      Mesh mesh{};
      std::vector<MeshPart> parts{};
      bool loaded{false};
      Renderer::Scene *scene{};

    public:
      void fromT3DM(const T3DM::T3DMData &t3dmData, Project::AssetManager &assetManager);

      void recreate(Renderer::Scene &sc);
      void draw(
        SDL_GPURenderPass* pass, SDL_GPUCommandBuffer *cmdBuff, UniformsObject &uniforms,
        const std::vector<uint32_t> &partsIndices,
        const UniformsOverrides& overrides = {}
      );

      const Utils::AABB& getAABB() const { return mesh.getAABB(); }
      bool isLoaded() const { return loaded; }
  };
}
