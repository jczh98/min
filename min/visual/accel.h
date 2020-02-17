#pragma once

#include "defs.h"
#include "geometry.h"
#include "intersection.h"

namespace min {

class Accelerator : public Unit {
 public:
  virtual void AddShape(const std::vector<std::shared_ptr<Shape>> &shape) = 0;
  virtual void Build() = 0;
  virtual bool Intersect(const Ray &ray, SurfaceIntersection &isect) const = 0;
  virtual bool IntersectP(const Ray &ray) const = 0;
  virtual Bounds3f  WorldBound() const = 0;
};
MIN_INTERFACE(Accelerator)

}


