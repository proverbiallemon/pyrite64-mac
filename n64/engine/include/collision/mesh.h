/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include <vector>
#include <string>
#include "shapes.h"

namespace P64
{
  class Object;
}

namespace Coll
{
  struct BVH;

  struct Mesh
  {
    // NOTE: don't place any extra members here!
    // mirrors the collion data in the t3dm extension
    uint32_t triCount{};
    uint32_t vertCount{};
    float collScale{};
    fm_vec3_t *verts{};
    IVec3 *normals{};
    BVH* bvh{};
    // data follows here: indices, normals, verts, BVH
    int16_t indices[];

    [[nodiscard]] CollInfo vsSphere(const BCS &sphere, const Triangle& triangle) const;
    [[nodiscard]] CollInfo vsBox(const BCS &box, const Triangle& triangle) const;
    [[nodiscard]] RaycastRes vsFloorRay(const fm_vec3_t &pos, const Triangle& triangle) const;

    static Mesh* load(void* rawData);
  };

  struct MeshInstance {
    Mesh *mesh{};
    P64::Object* object{};

    fm_vec3_t invScale{};
    fm_quat_t invRot{};
  };
}
