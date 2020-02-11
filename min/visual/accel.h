#pragma once

#include "defs.h"
#include "geometry.h"
#include "intersection.h"

namespace min {

class Accelerator : public Unit {
 public:
  virtual bool Intersect(const Ray &ray, SurfaceIntersection &isect) const = 0;
  virtual bool WorldBound() const = 0;
};
MIN_INTERFACE(Accelerator)

}


