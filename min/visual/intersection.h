#pragma once

#include "defs.h"
#include "frame.h"
#include "shape.h"

namespace min {

class Intersection {
 public:
  Point3 p;
  Float time;
  Vector3 wo;
  Frame geo_frame;

  Ray SpawnRay(const Vector3f &d) const {
    return Ray(p, d, kInfinity, time);
  }
};

class SurfaceIntersection : public Intersection {
 public:
  Frame shading_frame;
  const Shape *shape = nullptr;
  int face_index = 0;

  Vector3f ToLocal(const Vector3f &d) const {
    return shading_frame.ToLocal(d);
  }

  Vector3f ToWorld(const Vector3f &d) const {
    return shading_frame.ToWorld(d);
  }
};

}

