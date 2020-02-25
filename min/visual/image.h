#pragma once

#include "defs.h"
#include "geometry.h"

namespace min {

extern void WriteImage(const std::string &name, const Float *rgb,
                const Bounds2i &output_bounds, const Point2i &total_resolution);

extern std::unique_ptr<Vector3[]> ReadImage(const std::string &name, int &width, int &height);

}


