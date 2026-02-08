/**
* @copyright 2026 - Max Bebök
* @license MIT
*/
#pragma once
#include <string>
#include <filesystem>

namespace Utils
{
  class Toolchain
  {
    public:
      struct State
      {
        std::filesystem::path mingwPath{}; // empty if not found, always empty on linux
        bool hasToolchain{};
        bool hasLibdragon{};
        bool hasTiny3d{};
      };

    private:
      State state{};

    public:
      void scan();

      void install();
      bool isInstalling();

      bool runCmdSyncLogged(const std::string &cmd);

      const State& getState() const { return state; }
  };
}
