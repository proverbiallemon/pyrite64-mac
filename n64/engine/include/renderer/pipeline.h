/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <libdragon.h>

namespace P64
{
  class Scene;

  class RenderPipeline
  {
    protected:
      Scene &scene;

      void setupLayer();

      surface_t *surfColor{};
      surface_t *surfDepth{};

    public:
      explicit RenderPipeline(Scene &sc) : scene{sc} {}

      [[nodiscard]] surface_t* getCurrColorSurf() const { return surfColor; }
      [[nodiscard]] surface_t* getCurrDepthSurf() const { return surfDepth; }

      virtual ~RenderPipeline() = default;
      virtual void init() = 0;
      virtual void preDraw() = 0;
      virtual void draw() = 0;
  };

  class RenderPipelineDefault final : public RenderPipeline
  {
    private:
      surface_t surfFbColor[3]{};

    public:
      using RenderPipeline::RenderPipeline;
      ~RenderPipelineDefault() override;

      void init() override;
      void preDraw() override;
      void draw() override;
  };
}
