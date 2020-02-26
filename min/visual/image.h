#pragma once

#include "defs.h"
#include "geometry.h"

namespace min {

inline bool HasExtension(const std::string &value, const std::string &ending) {
  if (ending.size() > value.size()) {
    return false;
  }
  return std::equal(
      ending.rbegin(), ending.rend(), value.rbegin(),
      [](char a, char b) { return std::tolower(a) == std::tolower(b); });
}

extern void WriteImage(const std::string &name, const Float *rgb,
                const Bounds2i &output_bounds, const Point2i &total_resolution);

extern std::unique_ptr<Vector3[]> ReadImage(const std::string &name, int &width, int &height);

}


