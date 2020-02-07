#pragma once

#if defined(_MSC_VER)
#define NOMINMAX
#endif

#include <min-ray/util.h>
#include <fmt/format.h>

namespace min::ray {

// Forward declarations
class Scene;
class Integrator;
template <typename T>
class Vector2;
template <typename T>
class Vector3;
template <typename T>
class Point3;
template <typename T>
class Point2;
template <typename T>
class Normal3;
class Ray;
template <typename T>
class Bounds2;
template <typename T>
class Bounds3;
class Transform;
struct Intersection;
class SurfaceIntersection;
class Shape;
class Primitive;
class GeometricPrimitive;
class TransformedPrimitive;
template <int nSpectrumSamples>
class CoefficientSpectrum;
class RGBSpectrum;
class SampledSpectrum;
using Spectrum = RGBSpectrum;
class Camera;
struct CameraSample;
class Sampler;
class Filter;
class Film;
class FilmTile;
class BSDF;
class Texture;
class Light;
class VisibilityTester;
class AreaLight;
struct Distribution1D;
class Distribution2D;

#ifdef MIN_COMPILER_MSVC
#define kMaxFloat std::numeric_limits<Float>::max()
#define kInfinity std::numeric_limits<Float>::infinity()
#else
static constexpr Float kMaxFloat = std::numeric_limits<Float>::max();
static constexpr Float kInfinity = std::numeric_limits<Float>::infinity();
#endif
#ifdef MIN_COMPILER_MSVC
#define kMachineEpsilon (std::numeric_limits<Float>::epsilon() * 0.5)
#else
static constexpr Float kMachineEpsilon =
    std::numeric_limits<Float>::epsilon() * 0.5;
#endif

static constexpr Float kShadowEpsilon = 0.0001f;
static constexpr Float kPi = 3.14159265358979323846;
static constexpr Float kInvPi = 0.31830988618379067154;
static constexpr Float kInv2Pi = 0.15915494309189533577;
static constexpr Float kInv4Pi = 0.07957747154594766788;
static constexpr Float kPiOver2 = 1.57079632679489661923;
static constexpr Float kPiOver4 = 0.78539816339744830961;
static constexpr Float kSqrt2 = 1.41421356237309504880;

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

inline Float gamma(int n) {
  return (n * kMachineEpsilon) / (1 - n * kMachineEpsilon);
}

inline Float GammaCorrect(Float value) {
  if (value <= 0.0031308f) return 12.92f * value;
  return 1.055f * std::pow(value, (Float)(1.f / 2.4f)) - 0.055f;
}

template <typename T, typename U, typename V>
inline T Clamp(T val, U low, V high) {
  if (val < low)
    return low;
  else if (val > high)
    return high;
  else
    return val;
}

template <typename T>
inline T Mod(T a, T b) {
  T result = a - (a / b) * b;
  return (T)((result < 0) ? result + b : result);
}

template <>
inline Float Mod(Float a, Float b) {
  return std::fmod(a, b);
}

inline Float Radians(Float deg) { return (kPi / 180) * deg; }

inline Float Degrees(Float rad) { return (180 / kPi) * rad; }

inline Float Lerp(Float t, Float v1, Float v2) { return (1 - t) * v1 + t * v2; }

inline bool Quadratic(Float a, Float b, Float c, Float *t0, Float *t1) {
  // Find quadratic discriminant
  double discrim = (double)b * (double)b - 4 * (double)a * (double)c;
  if (discrim < 0) return false;
  double rootDiscrim = std::sqrt(discrim);

  // Compute quadratic _t_ values
  double q;
  if (b < 0)
    q = -.5 * (b - rootDiscrim);
  else
    q = -.5 * (b + rootDiscrim);
  *t0 = q / a;
  *t1 = c / q;
  if (*t0 > *t1) std::swap(*t0, *t1);
  return true;
}

template <typename Predicate>
int FindInterval(int size, const Predicate &pred) {
  int first = 0, len = size;
  while (len > 0) {
    int half = len >> 1, middle = first + half;
    // Bisect range based on value of _pred_ at _middle_
    if (pred(middle)) {
      first = middle + 1;
      len -= half + 1;
    } else
      len = half;
  }
  return Clamp(first - 1, 0, size - 2);
}

}