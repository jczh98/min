#pragma once

#include <min/common/util.h>
#include <cmath>

namespace min {

#undef max
#undef min

static constexpr Float kMaxFloat = std::numeric_limits<Float>::max();
static constexpr Float kInfinity = std::numeric_limits<Float>::infinity();
static constexpr Float kRayBias = 0.0001f;
static constexpr Float kPi = 3.14159265358979323846;
static constexpr Float kInvPi = 0.31830988618379067154;
static constexpr Float kInv2Pi = 0.15915494309189533577;
static constexpr Float kInv4Pi = 0.07957747154594766788;
static constexpr Float kPiOver2 = 1.57079632679489661923;
static constexpr Float kPiOver4 = 0.78539816339744830961;
static constexpr Float kSqrt2 = 1.41421356237309504880;

template<typename T>
MIN_FORCE_INLINE T Clamp(const T &a, const T &min, const T &max) noexcept {
  if (a < min)
    return min;
  if (a > max)
    return max;
  return a;
}

template<typename T>
MIN_FORCE_INLINE T Clamp(const T &a) noexcept {
  return Clamp(a, T(0), T(1));
}

template<typename T, typename V>
MIN_FORCE_INLINE V Lerp(T a, V x_0, V x_1) noexcept {
  return V((T(1) - a) * x_0 + a * x_1);
}

template<typename T>
MIN_FORCE_INLINE bool IsNormal(T m) noexcept {
  return std::isfinite(m);
}

template<typename T>
MIN_FORCE_INLINE bool Abnormal(T m) noexcept {
  return !IsNormal(m);
}

MIN_FORCE_INLINE Float Radians(Float deg) { return (kPi / 180) * deg; }

MIN_FORCE_INLINE Float Degrees(Float rad) { return (180 / kPi) * rad; }

}
