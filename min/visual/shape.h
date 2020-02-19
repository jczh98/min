#pragma once

#include "defs.h"
#include "geometry.h"
#include "transform.h"
#include "light.h"
#include "bsdf.h"

namespace min {

struct SurfaceSample {
  Float pdf;
  Point3 ref;
  Point3 p;
  Normal3 normal;
};

class Shape : public Unit {
 protected:
  Transform object2world, world2object;
 public:
  std::shared_ptr<Light> area_light = nullptr;
  std::shared_ptr<BSDF> bsdf = nullptr;
  Shape(const Transform &object2world, const Transform &world2object) : object2world(object2world), world2object(world2object) {}
  virtual Bounds3f WorldBound() const = 0;
  virtual Bounds3f ObjectBound() const = 0;
  virtual bool Intersect(const Ray &ray, SurfaceIntersection &isect) const = 0;
  virtual bool IntersectP(const Ray &ray) const = 0;
  virtual Float Area() const = 0;
  virtual void Sample(const Point2f& u, SurfaceSample &sample) const = 0;
  virtual Float Pdf(const Intersection &ref, const Vector3 &wi) const;
};
MIN_INTERFACE(Shape)

}
