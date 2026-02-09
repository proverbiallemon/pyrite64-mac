/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <string>

namespace Editor::Noti
{
  enum Type
  {
    INFO,
    SUCCESS,
    ERROR
  };

  void add(Type type, const std::string &text);
  void draw();
}
