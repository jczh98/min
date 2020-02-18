#pragma once

#include <min/math/math.h>

namespace min {

using Spectrum = Vector3f;

inline bool IsBlack(const Spectrum &s) {
  return !(s.x != 0.0 || s.y != 0.0 || s.z != 0.0);
}

}

