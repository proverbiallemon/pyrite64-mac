/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <string>

#include "assetManager.h"
#include "scene/sceneManager.h"
#include "simdjson.h"

namespace Project
{
  struct ProjectConf
  {
    std::string name{};
    std::string romName{};
    std::string pathEmu{};
    std::string pathN64Inst{};

    std::string serialize() const;
  };

  class Project
  {
    private:
      std::string path;
      std::string pathConfig;

      AssetManager assets{this};
      SceneManager scenes{this};

      void deserialize(const simdjson::simdjson_result<simdjson::dom::element> &doc);

    public:
      ProjectConf conf{};

      Project(const std::string &path);

      void save();

      AssetManager& getAssets() { return assets; }
      SceneManager& getScenes() { return scenes; }
      [[nodiscard]] const std::string &getPath() const { return path; }

  };
}
