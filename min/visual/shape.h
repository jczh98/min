#pragma once

#include "defs.h"
#include "geometry.h"

namespace min {

class Shape : public Unit {
  virtual Bounds3f WorldBound() const = 0;
  virtual Bounds3f ObjectBound() const = 0;
  virtual bool Intersect(const Ray &ray, SurfaceIntersection &isect) const = 0;
  virtual Float Area() const = 0;

};

}
