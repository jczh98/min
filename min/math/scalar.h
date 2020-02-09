#pragma once

#include <min/common/util.h>
#include <cmath>

namespace min {

#undef max
#undef min

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

}
