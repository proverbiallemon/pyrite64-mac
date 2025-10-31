/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "scene/lighting.h"

#include <t3d/t3d.h>

void P64::Lighting::apply() const
{
  int lightIdx = 0;
  color_t ambient{};
  for(uint32_t i=0; i<lightCount; ++i)
  {
    const auto &l = lights[i];
    if (l.strength < 0)
    {
      ambient.r += l.color.r;
      ambient.g += l.color.g;
      ambient.b += l.color.b;
      ambient.a += l.color.a;
    }else if(l.strength == 0)
    {
      t3d_light_set_directional(lightIdx, l.color, l.dirOrPos);
      ++lightIdx;
    } else {
      t3d_light_set_point(lightIdx,
        l.color,
        l.dirOrPos,
        l.strength
        // @TODO: ignore normals setting
      );
      ++lightIdx;
    }
  }

  t3d_light_set_ambient(ambient);
  t3d_light_set_count(lightIdx);
}
