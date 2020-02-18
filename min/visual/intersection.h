#pragma once

#include "defs.h"
#include "frame.h"
#include "texture.h"
#include "bsdf.h"

namespace min {

class Intersection {
 public:
  Point3 p;
  Float time;
  Vector3 wo;
  ShadingPoint sp;
  Frame geo_frame;

  Ray SpawnRay(const Vector3f &d) const {
    return Ray(p, d, kInfinity, time);
  }

  Ray SpwanRayTo(const Point3 &p2) const {
    return Ray(p, p2 - p, 1 - kShadowEpsilon, time);
  }

};

class SurfaceIntersection : public Intersection {
 public:
  Frame shading_frame;
  const Shape *shape = nullptr;
  BSDF *bsdf = nullptr;
  int face_index = 0;

  Vector3f ToLocal(const Vector3f &d) const {
    return shading_frame.ToLocal(d);
  }

  Vector3f ToWorld(const Vector3f &d) const {
    return shading_frame.ToWorld(d);
  }
};

}

