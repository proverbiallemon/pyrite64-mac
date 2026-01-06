/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "lib/math.h"

fm_quat_t P64::Math::unpackQuat(uint32_t quatPacked)
{
  constexpr int BITS = 10;
  constexpr int BIT_MASK = (1 << BITS) - 1;

  uint32_t largestIdx = quatPacked >> 30;
  uint32_t idx0 = (largestIdx + 1) & 0b11;
  uint32_t idx1 = (largestIdx + 2) & 0b11;
  uint32_t idx2 = (largestIdx + 3) & 0b11;

  float q0 = s10ToFloat((quatPacked >> (BITS * 2)) & BIT_MASK, -SQRT_2_INV, SQRT_2_INV*2);
  float q1 = s10ToFloat((quatPacked >> (BITS * 1)) & BIT_MASK, -SQRT_2_INV, SQRT_2_INV*2);
  float q2 = s10ToFloat((quatPacked >> (BITS * 0)) & BIT_MASK, -SQRT_2_INV, SQRT_2_INV*2);

  fm_quat_t q{};
  q.v[idx0] = q0;
  q.v[idx1] = q1;
  q.v[idx2] = q2;
  q.v[largestIdx] = sqrtf(1.0f - q0*q0 - q1*q1 - q2*q2);

  if(q.w > 0.9999f) // assume identity if close enough, makes things later on faster
  {
    q = {0,0,0,1};
  }

  return q;
}
