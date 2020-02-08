#pragma once

#include <min/common/util.h>
#include <functional>

namespace min {

// Reference for https://github.com/taichi-dev/taichi/blob/master/taichi/math/linalg.h

enum InstSetExt { kNone };

constexpr InstSetExt kDefaultInstructionSet = InstSetExt::kNone;

//******************************************************************************
//                           N dimensional vector
//******************************************************************************

template<int dim, typename T, InstSetExt ISE>
struct VectorNDBase {
  static constexpr bool kSimd = false;
  static constexpr int kStorageElements = dim;
  T d[dim];
};

template<typename T, InstSetExt ISE>
struct VectorNDBase<1, T, ISE> {
  static constexpr bool kSimd = false;
  static constexpr int kStorageElements = 1;
  union {
    T d[1];
    struct {
      T x;
    };
  };
};

template<typename T, InstSetExt ISE>
struct VectorNDBase<2, T, ISE> {
  static constexpr bool kSimd = false;
  static constexpr int kStorageElements = 2;
  union {
    T d[2];
    struct {
      T x, y;
    };
  };
};

template<typename T, InstSetExt ISE>
struct VectorNDBase<3, T, ISE> {
  static constexpr bool kSimd = false;
  static constexpr int kStorageElements = 3;
  union {
    T d[3];
    struct {
      T x, y, z;
    };
  };
};

template<typename T, InstSetExt ISE>
struct VectorNDBase<4, T, ISE> {
  static constexpr int kStorageElements = 4;
  static constexpr bool kSimd = false;
  union {
    T d[4];
    struct {
      T x, y, z, w;
    };
  };
};

template<int dim__, typename T, InstSetExt ISE = kDefaultInstructionSet>
struct VectorND : public VectorNDBase<dim__, T, ISE> {
  static constexpr int kDim = dim__;
  using ScalarType = T;
  using VectorBase = VectorNDBase<kDim, T, ISE>;
  using VectorBase::d;
  static constexpr int kStorageElements = VectorBase::kStorageElements;

  MIN_FORCE_INLINE VectorND() {
    for (int i = 0; i < kDim; i++) {
      this->d[i] = T(0);
    }
  }

  template<int dim_, typename T_, InstSetExt ISE_>
  explicit MIN_FORCE_INLINE VectorND(const VectorND<dim_, T_, ISE_> &o)
      : VectorND() {
    for (int i = 0; i < std::min(dim_, dim__); i++) {
      d[i] = o[i];
    }
  }

  explicit MIN_FORCE_INLINE VectorND(const std::array<T, kDim> &o) {
    for (int i = 0; i < kDim; i++) {
      d[i] = o[i];
    }
  }

  // Vector initialization
  template<typename F,
      std::enable_if_t<std::is_same<F, VectorND>::value, int> = 0>
  explicit MIN_FORCE_INLINE VectorND(const F &f) {
    for (int i = 0; i < kDim; i++)
      this->d[i] = f[i];
  }

  // Scalar initialization
  template<typename F, std::enable_if_t<std::is_same<F, T>::value, int> = 0>
  explicit MIN_FORCE_INLINE VectorND(const F &f) {
    for (int i = 0; i < kDim; i++)
      this->d[i] = f;
  }

  // Function intialization
  template<
      typename F,
      std::enable_if_t<std::is_convertible<F, std::function<T(int)>>::value,
                       int> = 0>
  explicit MIN_FORCE_INLINE VectorND(const F &f) {
    for (int i = 0; i < kDim; i++)
      this->d[i] = f(i);
  }

  template<int dim_ = kDim, typename T_ = T, InstSetExt ISE_ = ISE>
  explicit MIN_FORCE_INLINE VectorND(T v) {
    for (int i = 0; i < kDim; i++) {
      this->d[i] = v;
    }
  }

  explicit MIN_FORCE_INLINE VectorND(T v0, T v1) {
    static_assert(kDim == 2, "Vector dim must be 2");
    this->d[0] = v0;
    this->d[1] = v1;
  }

  // Vector3f
  template<int dim_ = kDim, typename T_ = T, InstSetExt ISE_ = ISE>
  explicit MIN_FORCE_INLINE VectorND(T v0, T v1, T v2) {
    static_assert(kDim == 3, "Vector dim must be 3");
    this->d[0] = v0;
    this->d[1] = v1;
    this->d[2] = v2;
  }

  // Vector4f
  template<int dim_ = kDim, typename T_ = T, InstSetExt ISE_ = ISE>
  explicit MIN_FORCE_INLINE VectorND(T v0, T v1, T v2, T v3) {
    static_assert(kDim == 4, "Vector dim must be 4");
    this->d[0] = v0;
    this->d[1] = v1;
    this->d[2] = v2;
    this->d[3] = v3;
  }

  // Vector extension
  template<int dim_ = kDim, std::enable_if_t<(dim_ > 1), int> = 0>
  explicit MIN_FORCE_INLINE VectorND(const VectorND<kDim - 1, T, ISE> &o,
                                     T extra) {
    for (int i = 0; i < dim_ - 1; i++) {
      this->d[i] = o[i];
    }
    this->d[dim - 1] = extra;
  }

  // Convert from std::vector
  template<typename T_>
  explicit MIN_FORCE_INLINE VectorND(const std::vector<T_> &o) {
    if (o.size() != kDim) {
      TC_ERROR("Dimension mismatch: " + std::to_string(kDim) + " v.s. " +
          std::to_string((int) o.size()));
    }
    for (int i = 0; i < kDim; i++)
      this->d[i] = T(o[i]);
  }

  MIN_FORCE_INLINE T &operator[](int i) {
    return this->d[i];
  }

  MIN_FORCE_INLINE const T &operator[](int i) const {
    return this->d[i];
  }

  MIN_FORCE_INLINE T &operator()(int i) {
    return d[i];
  }

  MIN_FORCE_INLINE const T &operator()(int i) const {
    return d[i];
  }

  MIN_FORCE_INLINE T Dot(VectorND<kDim, T, ISE> o) const {
    T ret = T(0);
    for (int i = 0; i < kDim; i++)
      ret += this->d[i] * o[i];
    return ret;
  }

  MIN_FORCE_INLINE VectorND &operator=(const VectorND &o) {
    memcpy(this, &o, sizeof(*this));
    return *this;
  }

  // Non-SIMD cases
  template<int dim_ = kDim, typename T_ = T, InstSetExt ISE_ = ISE>
  MIN_FORCE_INLINE VectorND operator+(const VectorND &o) const {
    return VectorND([=](int i) { return this->d[i] + o[i]; });
  }

  template<int dim_ = kDim, typename T_ = T, InstSetExt ISE_ = ISE>
  MIN_FORCE_INLINE VectorND operator-(const VectorND &o) const {
    return VectorND([=](int i) { return this->d[i] - o[i]; });
  }

  template<int dim_ = kDim, typename T_ = T, InstSetExt ISE_ = ISE>
  MIN_FORCE_INLINE VectorND operator*(const VectorND &o) const {
    return VectorND([=](int i) { return this->d[i] * o[i]; });
  }

  template<int dim_ = kDim, typename T_ = T, InstSetExt ISE_ = ISE>
  MIN_FORCE_INLINE VectorND operator/(const VectorND &o) const {
    return VectorND([=](int i) { return this->d[i] / o[i]; });
  }

  template<int dim_ = kDim, typename T_ = T, InstSetExt ISE_ = ISE>
  MIN_FORCE_INLINE VectorND operator%(const VectorND &o) const {
    return VectorND([=](int i) { return this->d[i] % o[i]; });
  }

// Inplace operations
  MIN_FORCE_INLINE VectorND &operator+=(const VectorND &o) {
    (*this) = (*this) + o;
    return *this;
  }

  MIN_FORCE_INLINE VectorND &operator-=(const VectorND &o) {
    (*this) = (*this) - o;
    return *this;
  }

  MIN_FORCE_INLINE VectorND &operator*=(const VectorND &o) {
    (*this) = (*this) * o;
    return *this;
  }

  MIN_FORCE_INLINE VectorND &operator*=(const T &o) {
    (*this) = (*this) * o;
    return *this;
  }

  MIN_FORCE_INLINE VectorND &operator/=(const VectorND &o) {
    (*this) = (*this) / o;
    return *this;
  }

  MIN_FORCE_INLINE VectorND &operator/=(const T &o) {
    (*this) = (*this) / o;
    return *this;
  }

  MIN_FORCE_INLINE VectorND operator-() const {
    return VectorND([=](int i) { return -this->d[i]; });
  }

  MIN_FORCE_INLINE bool operator==(const VectorND &o) const {
    for (int i = 0; i < kDim; i++)
      if (this->d[i] != o[i])
        return false;
    return true;
  }

  MIN_FORCE_INLINE bool operator<(const VectorND &o) const {
    for (int i = 0; i < kDim; i++)
      if (this->d[i] >= o[i])
        return false;
    return true;
  }

  MIN_FORCE_INLINE bool operator<=(const VectorND &o) const {
    for (int i = 0; i < kDim; i++)
      if (this->d[i] > o[i])
        return false;
    return true;
  }

  MIN_FORCE_INLINE bool operator>(const VectorND &o) const {
    for (int i = 0; i < kDim; i++)
      if (this->d[i] <= o[i])
        return false;
    return true;
  }

  MIN_FORCE_INLINE bool operator>=(const VectorND &o) const {
    for (int i = 0; i < kDim; i++)
      if (this->d[i] < o[i])
        return false;
    return true;
  }

  MIN_FORCE_INLINE bool operator==(const std::vector<T> &o) const {
    if (o.size() != kDim)
      return false;
    for (int i = 0; i < kDim; i++)
      if (this->d[i] != o[i])
        return false;
    return true;
  }

  MIN_FORCE_INLINE bool operator!=(const VectorND &o) const {
    for (int i = 0; i < kDim; i++)
      if (this->d[i] != o[i])
        return true;
    return false;
  }

  MIN_FORCE_INLINE T MinComp() const {
    T ret = this->d[0];
    for (int i = 1; i < kDim; i++) {
      ret = std::min(ret, this->d[i]);
    }
    return ret;
  }

  MIN_FORCE_INLINE T MaxComp() const {
    T ret = this->d[0];
    for (int i = 1; i < kDim; i++) {
      ret = std::max(ret, this->d[i]);
    }
    return ret;
  }

  template<int dim_ = kDim, typename T_ = T, InstSetExt ISE_ = ISE>
  MIN_FORCE_INLINE T LengthSquared() const {
    T ret = 0;
    for (int i = 0; i < kDim; i++) {
      ret += this->d[i] * this->d[i];
    }
    return ret;
  }

  template<int dim_ = kDim, typename T_ = T, InstSetExt ISE_ = ISE>
  MIN_FORCE_INLINE T Length() const {
    return std::sqrt(LengthSquared());
  }

  std::string ToString() const {
    if (kDim == 1) return fmt::format("[{}]", d[0]);
    if (kDim == 2) return fmt::format("[{} {}]", d[0], d[1]);
    if (kDim == 3) return fmt::format("[{} {} {}]", d[0], d[1], d[2]);
    if (kDim == 4) return fmt::format("[{} {} {} {}]", d[0], d[1], d[2], d[3]);
  }
};

template<typename T, int dim, InstSetExt ISE = kDefaultInstructionSet>
using TVector = VectorND<dim, T, ISE>;

template<int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE VectorND<dim, T, ISE> operator
*(T a, const VectorND<dim, T, ISE> &v) {
  return VectorND<dim, T, ISE>(a) * v;
}

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE VectorND<dim, T, ISE> operator*(const VectorND<dim, T, ISE> &v,
                                                T a) {
  return a * v;
}

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE VectorND<dim, T, ISE> operator/(
    T a,
const VectorND<dim, T, ISE> &v) {
  return VectorND<dim, T, ISE>(a) / v;
}

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE VectorND<dim, T, ISE> operator/(const VectorND<dim, T, ISE> &v,
                                                T a) {
  return v / VectorND<dim, T, ISE>(a);
}

template <typename T, InstSetExt ISE>
MIN_FORCE_INLINE T Cross(const VectorND<2, T, ISE> &a,
                        const VectorND<2, T, ISE> &b) {
  return a.x * b.y - a.y * b.x;
}

template <typename T, InstSetExt ISE>
MIN_FORCE_INLINE VectorND<3, T, ISE> Cross(const VectorND<3, T, ISE> &a,
                                          const VectorND<3, T, ISE> &b) {
  return VectorND<3, T, ISE>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
      a.x * b.y - a.y * b.x);
}

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE T Dot(const VectorND<dim, T, ISE> &a,
                      const VectorND<dim, T, ISE> &b) {
  return a.Dot(b);
}

using Vector1 = VectorND<1, Float, kDefaultInstructionSet>;
using Vector2 = VectorND<2, Float, kDefaultInstructionSet>;
using Vector3 = VectorND<3, Float, kDefaultInstructionSet>;
using Vector4 = VectorND<4, Float, kDefaultInstructionSet>;

using Vector1f = VectorND<1, float32, kDefaultInstructionSet>;
using Vector2f = VectorND<2, float32, kDefaultInstructionSet>;
using Vector3f = VectorND<3, float32, kDefaultInstructionSet>;
using Vector4f = VectorND<4, float32, kDefaultInstructionSet>;

using Vector1d = VectorND<1, float64, kDefaultInstructionSet>;
using Vector2d = VectorND<2, float64, kDefaultInstructionSet>;
using Vector3d = VectorND<3, float64, kDefaultInstructionSet>;
using Vector4d = VectorND<4, float64, kDefaultInstructionSet>;

using Vector1i = VectorND<1, int, kDefaultInstructionSet>;
using Vector2i = VectorND<2, int, kDefaultInstructionSet>;
using Vector3i = VectorND<3, int, kDefaultInstructionSet>;
using Vector4i = VectorND<4, int, kDefaultInstructionSet>;

}


