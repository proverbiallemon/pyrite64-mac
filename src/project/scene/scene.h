/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "object.h"
#include "../../utils/color.h"

namespace Project
{
  struct SceneConf
  {
    std::string name{};
    int fbWidth{320};
    int fbHeight{240};
    int fbFormat{0};
    Utils::Color clearColor{};
    bool doClearColor{};
    bool doClearDepth{};

    std::string serialize() const;
  };

  class Scene
  {
    private:
      int id{};
      Object root{nullptr};

    public:
      SceneConf conf{};

      Scene(int id_);

      void save();
      Object& getRootObject() { return root; }
      std::unordered_map<uint32_t, std::shared_ptr<Object>> objectsMap{};

      std::shared_ptr<Object> addObject(Object &parent);
      void removeObject(Object &obj);

      std::shared_ptr<Object> getObjectByUUID(uint32_t uuid) {
        if (objectsMap.contains(uuid)) {
          return objectsMap[uuid];
        }
        return nullptr;
      }

      std::string serialize();
      void deserialize(const std::string &data);
  };
}