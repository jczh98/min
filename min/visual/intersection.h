#pragma once

#include "defs.h"
#include "frame.h"
#include "texture.h"
#include "material.h"

namespace min {

class Intersection {
 public:
  Point3 p;
  Float time;
  Vector3 error;
  Vector3 wo;
  ShadingPoint sp;
  Frame geo_frame;

  Ray SpawnRay(const Vector3f &d) const {
    Point3 origin = OffsetRayOrigin(p, error, geo_frame.n, d);
    return Ray(origin, d, kInfinity, time);
  }

  Ray SpwanRayTo(const Point3 &p2) const {
    Vector3 d = p2 - p;
    Point3 origin = OffsetRayOrigin(p, error, geo_frame.n, d);
    return Ray(origin, d, 1 - kShadowEpsilon, time);
  }

  Ray SpwanRayTo(const Intersection &it) const {
    Point3 origin = OffsetRayOrigin(p, error, geo_frame.n, it.p - p);
    Point3 target = OffsetRayOrigin(it.p, it.error, it.geo_frame.n, origin - it.p);
    Vector3 d = target - origin;
    return Ray(origin, d, 1 - kShadowEpsilon, time);
  }
};

class SurfaceIntersection : public Intersection {
 public:
  Frame shading_frame;
  const Shape *shape = nullptr;
  std::unique_ptr<BSDF> bsdf = nullptr;
  int face_index = 0;

  Vector3f ToLocal(const Vector3f &d) const {
    return shading_frame.ToLocal(d);
  }

  Vector3f ToWorld(const Vector3f &d) const {
    return shading_frame.ToWorld(d);
  }

  void ComputeScatteringEvents();

};

}

