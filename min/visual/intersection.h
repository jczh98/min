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
};

class SurfaceIntersection : public Intersection {
 public:
  Frame shading_frame;
  const Shape *shape = nullptr;
  int face_index = 0;
};

}

