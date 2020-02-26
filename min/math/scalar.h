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
static constexpr Float kMachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5;

template<typename T>
MIN_FORCE_INLINE T Mod(const T &a, const T &b) noexcept {
  T result = a - (a / b) * b;
  return (T)((result < 0) ? result + b : result);
}

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

MIN_FORCE_INLINE Float Gamma(int n) {
  return (n * kMachineEpsilon) / (1 - n * kMachineEpsilon);
}

MIN_FORCE_INLINE Float GammaCorrect(Float value) {
  if (value <= 0.0031308f) return 12.92f * value;
  return 1.055f * std::pow(value, Float(1.0f / 2.4f)) - 0.055f;
}

MIN_FORCE_INLINE Float Radians(Float deg) { return (kPi / 180) * deg; }

MIN_FORCE_INLINE Float Degrees(Float rad) { return (180 / kPi) * rad; }

MIN_FORCE_INLINE Float Log2(Float x) {
  const Float inv_log2 = 1.442695040888963387004650940071;
  return std::log(x) * inv_log2;
}

template <typename T>
MIN_FORCE_INLINE int FindInterval(int size, const T &pred) {
  int first = 0, len = size;
  while (len > 0) {
    int half = len >> 1, middle = first + half;
    if (pred(middle)) {
      first = middle + 1;
      len -= half + 1;
    } else  len = half;
  }
  return Clamp(first - 1, 0, size - 2);
}

template <typename T>
MIN_FORCE_INLINE constexpr bool IsPowerOf2(T v) {
  return v && !(v & (v - 1));
}

MIN_FORCE_INLINE constexpr int32_t RoundUpPow2(int32_t v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  return v + 1;
}

MIN_FORCE_INLINE int Log2Int(uint32_t v) {
#if defined(MIN_COMPILER_MSVC)
  unsigned long lz = 0;
  if (_BitScanReverse(&lz, v)) return lz;
  return 0;
#else
  return 31 - __builtin_clz(v);
#endif
}

}
