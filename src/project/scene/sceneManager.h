/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <string>
#include <vector>

namespace Project
{
  struct SceneEntry
  {
    int id{};
    std::string name;
  };

  class SceneManager
  {
    private:
      std::vector<SceneEntry> entries{};

    public:
      SceneManager();

      void reload();
      void save();

      [[nodiscard]] const std::vector<SceneEntry> &getEntries() const { return entries; }

      void add();
  };
}