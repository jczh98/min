#pragma once

#include "defs.h"
#include "geometry.h"
#include "transform.h"

namespace min {

struct SurfaceSample {
  Float pdf;
};

class Shape : public Unit {
  Transform object2world, world2object;
 public:
  virtual Bounds3f WorldBound() const = 0;
  virtual Bounds3f ObjectBound() const = 0;
  virtual bool Intersect(const Ray &ray, SurfaceIntersection &isect) const = 0;
  virtual Float Area() const = 0;
  virtual void Sample(const Point2f& u, SurfaceSample &sample) const = 0;
};
MIN_INTERFACE(Shape)

}
