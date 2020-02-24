#pragma once

#include <min/common/util.h>
#include <min/common/interface.h>
#include <min/common/json.h>
#include <min/common/resolver.h>
#include <min/math/math.h>
#include <filesystem>

namespace min {

using Point3f = Vector3f;
using Point3 = Vector3;
using Point3d = Vector3d;
using Point3i = Vector3i;
using Point2i = Vector2i;
using Point2f = Vector2f;
using Point2 = Vector2;

using Normal3f = Vector3f;
using Normal3 = Vector3;
using Normal3d = Vector3d;
using Normal3i = Vector3i;
using Normal2i = Vector2i;
using Normal2f = Vector2f;

// Forward declaration
class Film;
class FilmTile;
class Intersection;
class SurfaceIntersection;
class Scene;
class Shape;
class VisibilityTester;
class BSDFSample;

namespace fs = std::filesystem;

static constexpr Float kShadowEpsilon = 0.0001f;

#ifndef HAVE_HEX_FP_CONSTANTS
static const double kDoubleOneMinusEpsilon = 0.99999999999999989;
static const float kFloatOneMinusEpsilon = 0.99999994;
#else
static const double kDoubleOneMinusEpsilon = 0x1.fffffffffffffp-1;
static const float kFloatOneMinusEpsilon = 0x1.fffffep-1;
#endif

#ifdef MIN_USE_DOUBLE
static const Float kOneMinusEpsilon = kDoubleOneMinusEpsilon;
#else
static const Float kOneMinusEpsilon = kFloatOneMinusEpsilon;
#endif

inline Resolver *GetFileResolver() {
  static Resolver *resolver = new Resolver;
  return resolver;
}

inline uint32_t FloatToBits(float f) {
  uint32_t ui;
  memcpy(&ui, &f, sizeof(float));
  return ui;
}

inline float BitsToFloat(uint32_t ui) {
  float f;
  memcpy(&f, &ui, sizeof(uint32_t));
  return f;
}

inline uint64_t FloatToBits(double f) {
  uint64_t ui;
  memcpy(&ui, &f, sizeof(double));
  return ui;
}

inline double BitsToFloat(uint64_t ui) {
  double f;
  memcpy(&f, &ui, sizeof(uint64_t));
  return f;
}

inline float NextFloatUp(float v) {
  // Handle infinity and negative zero for _NextFloatUp()_
  if (std::isinf(v) && v > 0.) return v;
  if (v == -0.f) v = 0.f;

  // Advance _v_ to next higher float
  uint32_t ui = FloatToBits(v);
  if (v >= 0)
    ++ui;
  else
    --ui;
  return BitsToFloat(ui);
}

inline float NextFloatDown(float v) {
  // Handle infinity and positive zero for _NextFloatDown()_
  if (std::isinf(v) && v < 0.) return v;
  if (v == 0.f) v = -0.f;
  uint32_t ui = FloatToBits(v);
  if (v > 0)
    --ui;
  else
    ++ui;
  return BitsToFloat(ui);
}

inline double NextFloatUp(double v, int delta = 1) {
  if (std::isinf(v) && v > 0.) return v;
  if (v == -0.f) v = 0.f;
  uint64_t ui = FloatToBits(v);
  if (v >= 0.)
    ui += delta;
  else
    ui -= delta;
  return BitsToFloat(ui);
}

inline double NextFloatDown(double v, int delta = 1) {
  if (std::isinf(v) && v < 0.) return v;
  if (v == 0.f) v = -0.f;
  uint64_t ui = FloatToBits(v);
  if (v > 0.)
    ui -= delta;
  else
    ui += delta;
  return BitsToFloat(ui);
}

}


