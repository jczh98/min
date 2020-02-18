#pragma once

#include "defs.h"

namespace min {

inline Point2f ConcentricSampleDisk(const Point2f &u) {
  Point2f u_offset = 2.0f * u - Vector2f(1, 1);
  if (u_offset.x == 0 && u_offset.y == 0) return Point2f(0, 0);
  Float theta, r;
  if (std::abs(u_offset.x) > std::abs(u_offset.y)) {
    r = u_offset.x;
    theta = kPiOver4 * (u_offset.y / u_offset.x);
  } else {
    r = u_offset.y;
    theta = kPiOver2 - kPiOver4 * (u_offset.x / u_offset.y);
  }
  return r * Point2f(std::cos(theta), std::sin(theta));
}

inline Vector3f CosineSampleHemisphere(const Point2f &u) {
  Point2f d = ConcentricSampleDisk(u);
  Float z = std::sqrt(std::max((Float)0, 1 - d.x * d.x - d.y * d.y));
  return Vector3f(d.x, d.y, z);
}

inline Point2f UniformSampleTriangle(const Point2f &u) {
  Float su0 = std::sqrt(u[0]);
  return Point2f(1 - su0, u[1] * su0);
}

inline Float PowerHeuristic(int nf, Float fpdf, int ng, Float gpdf) {
  Float f = nf * fpdf, g = ng * gpdf;
  return (f * f) / (f * f + g * g);
}

}


