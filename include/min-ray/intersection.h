#pragma once

#include <min-ray/common.h>
#include <min-ray/geometry.h>
#include <min-ray/frame.h>

namespace min::ray {

struct Intersection {
  Point3f p;
  Float time;
  Vector3f wo;
  Vector3f pError;
  Frame geo_frame;

  Ray SpawnRay(const Vector3f &d) const {
    Point3f o = OffsetRayOrigin(p, pError, geo_frame.n, d);
    return Ray(o, d, kInfinity, time);
  }
  Ray SpawnRayTo(const Point3f &p2) const {
    Point3f origin = OffsetRayOrigin(p, pError, geo_frame.n, p2 - p);
    Vector3f d = p2 - p;
    return Ray(origin, d, 1 - kShadowEpsilon, time);
  }
  Ray SpawnRayTo(const Intersection &it) const {
    Point3f origin = OffsetRayOrigin(p, pError, geo_frame.n, it.p - p);
    Point3f target = OffsetRayOrigin(it.p, it.pError, it.geo_frame.n, origin - it.p);
    Vector3f d = target - origin;
    return Ray(origin, d, 1 - kShadowEpsilon, time);
  }
};

class SurfaceIntersection : public Intersection {
 public:
  Point2f uv;
  const Shape *shape = nullptr;
  BSDF *bsdf = nullptr;
  Frame shading_frame;
};
}