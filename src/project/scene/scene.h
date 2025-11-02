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

namespace Project
{
  struct SceneConf
  {
    std::string name{};
    int fbWidth{320};
    int fbHeight{240};
    int fbFormat{0};
    glm::vec4 clearColor{};
    bool doClearColor{};
    bool doClearDepth{};

    std::string serialize() const;
  };

  class Scene
  {
    private:
      int id{};
      Object root{};
      std::string scenePath{};

    public:
      SceneConf conf{};

      Scene(int id_, const std::string &projectPath);

      void save();
      Object& getRootObject() { return root; }
      std::unordered_map<uint32_t, std::shared_ptr<Object>> objectsMap{};

      std::shared_ptr<Object> addObject(Object &parent);
      std::shared_ptr<Object> addObject(Object &parent, std::shared_ptr<Object> obj);
      void removeObject(Object &obj);
      void removeAllObjects();

      bool moveObject(uint32_t uuidObject, uint32_t uuidTarget, bool asChild);

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