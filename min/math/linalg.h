#pragma once

#include "scalar.h"
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
    for (int i = 0; i < std::min<int>(dim_, dim__); i++) {
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
    this->d[kDim - 1] = extra;
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

  bool IsNormal() const {
    for (int i = 0; i < kDim; i++) {
      if (!min::IsNormal(this->d[i]))
        return false;
    }
    return true;
  }

  bool Abnormal() const {
    return !this->IsNormal();
  }

  std::string ToString() const {
    if (kDim == 1) return fmt::format("[{}]", d[0]);
    if (kDim == 2) return fmt::format("[{} {}]", d[0], d[1]);
    if (kDim == 3) return fmt::format("[{} {} {}]", d[0], d[1], d[2]);
    if (kDim == 4) return fmt::format("[{} {} {} {}]", d[0], d[1], d[2], d[3]);
  }
};

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

template <int dim, typename T>
MIN_FORCE_INLINE VectorND<dim, T> Min(const VectorND<dim, T> &a,
                                     const VectorND<dim, T> &b) {
  VectorND<dim, T> ret;
  for (int i = 0; i < dim; i++) {
    ret[i] = std::min(a[i], b[i]);
  }
  return ret;
}

template <int dim, typename T>
MIN_FORCE_INLINE VectorND<dim, T> Max(const VectorND<dim, T> &a,
                                     const VectorND<dim, T> &b) {
  VectorND<dim, T> ret;
  for (int i = 0; i < dim; i++) {
    ret[i] = std::max(a[i], b[i]);
  }
  return ret;
}

template <int dim, typename T>
MIN_FORCE_INLINE VectorND<dim, T> Ceil(const VectorND<dim, T> &v) {
  VectorND<dim, T> ret;
  for (int i = 0; i < dim; i++) {
    ret[i] = std::ceil(v[i]);
  }
  return ret;
}

template <int dim, typename T>
MIN_FORCE_INLINE VectorND<dim, T> Floor(const VectorND<dim, T> &v) {
  VectorND<dim, T> ret;
  for (int i = 0; i < dim; i++) {
    ret[i] = std::floor(v[i]);
  }
  return ret;
}

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE VectorND<dim, T, ISE> Normalize(
    const VectorND<dim, T, ISE> &a) {
  return (T(1) / a.Length()) * a;
}

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE VectorND<dim, T, ISE> Permute(
    const VectorND<dim, T, ISE> &a, const VectorND<dim, int, ISE> &permute) {
  VectorND<dim, T> ret;
  for (int i = 0; i < dim; i++) {
    ret[i] = a[permute[i]];
  }
  return ret;
}

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE VectorND<dim, T, ISE> Abs(
    const VectorND<dim, T, ISE> &a) {
  VectorND<dim, T> ret;
  for (int i = 0; i < dim; i++) {
    ret[i] = std::abs(a[i]);
  }
  return ret;
}

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE VectorND<dim, T, ISE> MaxComp(
    const VectorND<dim, T, ISE> &a) {
  return a.MaxComp();
}

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE VectorND<dim, T, ISE> MinComp(
    const VectorND<dim, T, ISE> &a) {
  return a.MinComp();
}

template <typename T, int dim, InstSetExt ISE = kDefaultInstructionSet>
using TVector = VectorND<dim, T, ISE>;

template <typename T, InstSetExt ISE = kDefaultInstructionSet>
using TVector2 = VectorND<2, T, ISE>;
template <typename T, InstSetExt ISE = kDefaultInstructionSet>
using TVector3 = VectorND<3, T, ISE>;
template <typename T, InstSetExt ISE = kDefaultInstructionSet>
using TVector4 = VectorND<4, T, ISE>;

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

//******************************************************************************
//                           N dimensional matrix
//******************************************************************************

template <int dim__, typename T, InstSetExt ISE = kDefaultInstructionSet>
struct MatrixND {
  static constexpr int kDim = dim__;
  using Vector = VectorND<kDim, T, ISE>;
  Vector d[kDim];

  MIN_FORCE_INLINE static MatrixND Identidy() {
    return MatrixND(1.0_f);
  }

  MIN_FORCE_INLINE MatrixND() {
    for (int i = 0; i < kDim; i++) {
      d[i] = VectorND<kDim, T, ISE>();
    }
  }

  template <int dim_, typename T_, InstSetExt ISE_>
  MIN_FORCE_INLINE explicit MatrixND(const MatrixND<dim_, T_, ISE_> &o)
      : MatrixND() {
    for (int i = 0; i < std::min(dim_, dim__); i++) {
      for (int j = 0; j < std::min(dim_, dim__); j++) {
        d[i][j] = o[i][j];
      }
    }
  }

  MIN_FORCE_INLINE MatrixND(T v) : MatrixND() {
    for (int i = 0; i < kDim; i++) {
      d[i][i] = v;
    }
  }

  MIN_FORCE_INLINE MatrixND(const MatrixND &o) {
    *this = o;
  }

  // Diagonal assigment
  MIN_FORCE_INLINE explicit MatrixND(Vector v) : MatrixND() {
    for (int i = 0; i < kDim; i++)
      this->d[i][i] = v[i];
  }

  MIN_FORCE_INLINE explicit MatrixND(Vector v0, Vector v1) {
    static_assert(kDim == 2, "Matrix dim must be 2");
    this->d[0] = v0;
    this->d[1] = v1;
  }

  MIN_FORCE_INLINE explicit MatrixND(Vector v0, Vector v1, Vector v2) {
    static_assert(kDim == 3, "Matrix dim must be 3");
    this->d[0] = v0;
    this->d[1] = v1;
    this->d[2] = v2;
  }

  MIN_FORCE_INLINE explicit MatrixND(Float t00, Float t01, Float t02, Float t03,
                                     Float t10, Float t11, Float t12, Float t13,
                                     Float t20, Float t21, Float t22, Float t23,
                                     Float t30, Float t31, Float t32, Float t33) {
    static_assert(kDim == 4, "Matrix dim must be 4");
    this->d[0] = Vector(t00, t01, t02, t03);
    this->d[1] = Vector(t10, t11, t12, t13);
    this->d[2] = Vector(t20, t21, t22, t23);
    this->d[3] = Vector(t30, t31, t32, t33);
  }

  MIN_FORCE_INLINE explicit MatrixND(Vector v0,
                                    Vector v1,
                                    Vector v2,
                                    Vector v3) {
    static_assert(kDim == 4, "Matrix dim must be 4");
    this->d[0] = v0;
    this->d[1] = v1;
    this->d[2] = v2;
    this->d[3] = v3;
  }

  // Function intialization
  template <
      typename F,
      std::enable_if_t<std::is_convertible<
          F,
          std::function<VectorND<dim__, T, ISE>(int)>>::value,
                       int> = 0>
  MIN_FORCE_INLINE explicit MatrixND(const F &f) {
    for (int i = 0; i < kDim; i++)
      this->d[i] = f(i);
  }

  MIN_FORCE_INLINE MatrixND &operator=(const MatrixND &o) {
    for (int i = 0; i < kDim; i++) {
      this->d[i] = o[i];
    }
    return *this;
  }

  MIN_FORCE_INLINE VectorND<kDim, T, ISE> &operator[](int i) {
    return d[i];
  }

  MIN_FORCE_INLINE T &operator()(int i, int j) {
    return d[j][i];
  }

  MIN_FORCE_INLINE const T &operator()(int i, int j) const {
    return d[j][i];
  }

  MIN_FORCE_INLINE const VectorND<kDim, T, ISE> &operator[](int i) const {
    return d[i];
  }

  template <int dim_ = kDim, typename T_ = T, InstSetExt ISE_ = ISE>
  MIN_FORCE_INLINE VectorND<kDim, T, ISE> operator*(
      const VectorND<kDim, T, ISE> &o) const {
    VectorND<kDim, T, ISE> ret = d[0] * o[0];
    for (int i = 1; i < kDim; i++)
      ret += d[i] * o[i];
    return ret;
  }

  template <int dim_ = kDim, typename T_ = T, InstSetExt ISE_ = ISE>
  MIN_FORCE_INLINE MatrixND operator*(const MatrixND &o) const {
    MatrixND ret;
    for (int i = 0; i < kDim; i++) {
      for (int j = 0; j < kDim; j++) {
        T tmp = 0;
        for (int k = 0; k < kDim; k++) {
          tmp += (*this)[i][k] * o[k][j];
        }
        ret[i][j] = tmp;
      }
    }
    return ret;
  }

  MIN_FORCE_INLINE MatrixND operator+(const MatrixND &o) const {
    return MatrixND([=](int i) { return this->d[i] + o[i]; });
  }

  MIN_FORCE_INLINE MatrixND operator-(const MatrixND &o) const {
    return MatrixND([=](int i) { return this->d[i] - o[i]; });
  }

  MIN_FORCE_INLINE MatrixND &operator+=(const MatrixND &o) {
    return this->set([&](int i) { return this->d[i] + o[i]; });
  }

  MIN_FORCE_INLINE MatrixND &operator-=(const MatrixND &o) {
    return this->set([&](int i) { return this->d[i] - o[i]; });
  }

  MIN_FORCE_INLINE MatrixND operator-() const {
    return MatrixND([=](int i) { return -this->d[i]; });
  }

  MIN_FORCE_INLINE bool operator==(const MatrixND &o) const {
    for (int i = 0; i < kDim; i++)
      for (int j = 0; j < kDim; j++)
        if (d[i][j] != o[i][j])
          return false;
    return true;
  }

  MIN_FORCE_INLINE bool operator!=(const MatrixND &o) const {
    for (int i = 0; i < kDim; i++)
      for (int j = 0; j < kDim; j++)
        if (d[i][j] != o[i][j])
          return true;
    return false;
  }

  MIN_FORCE_INLINE MatrixND Transpose() const {
    MatrixND ret;
    for (int i = 0; i < kDim; i++) {
      for (int j = 0; j < kDim; j++) {
        ret[i][j] = d[j][i];
      }
    }
    return ret;
  }

  MIN_FORCE_INLINE MatrixND ElementwiseProduct(const MatrixND<kDim, T> &o) const {
    MatrixND ret;
    for (int i = 0; i < kDim; i++) {
      ret[i] = this->d[i] * o[i];
    }
    return ret;
  }

  std::string ToString() const {
    if (kDim == 4) {
      return fmt::format("[{} {} {} {}\n"
                  " {} {} {} {}\n"
                  " {} {} {} {}\n"
                  " {} {} {} {}]",
                  d[0][0], d[0][1], d[0][2], d[0][3],
                  d[1][0], d[1][1], d[1][2], d[1][3],
                  d[2][0], d[2][1], d[2][2], d[2][3],
                  d[3][0], d[3][1], d[3][2], d[3][3]);
    }
    if (kDim == 3) {
      return fmt::format("[{} {} {}\n"
                         " {} {} {}\n"
                         " {} {} {}]",
                         d[0][0], d[0][1], d[0][2],
                         d[1][0], d[1][1], d[1][2],
                         d[2][0], d[2][1], d[2][2]);
    }
    if (kDim == 2) {
      return fmt::format("[{} {}\n"
                         " {} {}]",
                         d[0][0], d[0][1],
                         d[1][0], d[1][1]);
    }
  }
};

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE MatrixND<dim, T, ISE> operator
*(const T a, const MatrixND<dim, T, ISE> &M) {
  MatrixND<dim, T, ISE> ret;
  for (int i = 0; i < dim; i++) {
    ret[i] = a * M[i];
  }
  return ret;
}

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE MatrixND<dim, T, ISE> operator*(const MatrixND<dim, T, ISE> &M,
                                                const T a) {
  return a * M;
}

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE MatrixND<dim, T, ISE> Transpose(
    const MatrixND<dim, T, ISE> &mat) {
  return mat.Transpose();
}

template <typename T, int dim, InstSetExt ISE = kDefaultInstructionSet>
using TMatrix = MatrixND<dim, T, ISE>;

using Matrix2 = MatrixND<2, Float, kDefaultInstructionSet>;
using Matrix3 = MatrixND<3, Float, kDefaultInstructionSet>;
using Matrix4 = MatrixND<4, Float, kDefaultInstructionSet>;

using Matrix2f = MatrixND<2, float32, kDefaultInstructionSet>;
using Matrix3f = MatrixND<3, float32, kDefaultInstructionSet>;
using Matrix4f = MatrixND<4, float32, kDefaultInstructionSet>;

using Matrix2d = MatrixND<2, float64, kDefaultInstructionSet>;
using Matrix3d = MatrixND<3, float64, kDefaultInstructionSet>;
using Matrix4d = MatrixND<4, float64, kDefaultInstructionSet>;

template <typename T, InstSetExt ISE>
MIN_FORCE_INLINE Float Determinant(const MatrixND<2, T, ISE> &mat) {
  return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
}

template <typename T, InstSetExt ISE>
MIN_FORCE_INLINE T Determinant(const MatrixND<3, T, ISE> &mat) {
  return mat[0][0] * (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) -
      mat[1][0] * (mat[0][1] * mat[2][2] - mat[2][1] * mat[0][2]) +
      mat[2][0] * (mat[0][1] * mat[1][2] - mat[1][1] * mat[0][2]);
}

template <InstSetExt ISE, typename T>
MIN_FORCE_INLINE MatrixND<2, T, ISE> Inverse(const MatrixND<2, T, ISE> &mat) {
  T det = Determinant(mat);
  return static_cast<T>(1) / det *
      MatrixND<2, T, ISE>(VectorND<2, T, ISE>(mat[1][1], -mat[0][1]),
      VectorND<2, T, ISE>(-mat[1][0], mat[0][0]));
}

template <InstSetExt ISE, typename T>
MatrixND<3, T, ISE> Inverse(const MatrixND<3, T, ISE> &mat) {
  T det = Determinant(mat);
  return T(1.0) / det *
      MatrixND<3, T, ISE>(
      VectorND<3, T, ISE>(mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2],
                          mat[2][1] * mat[0][2] - mat[0][1] * mat[2][2],
                          mat[0][1] * mat[1][2] - mat[1][1] * mat[0][2]),
          VectorND<3, T, ISE>(mat[2][0] * mat[1][2] - mat[1][0] * mat[2][2],
                              mat[0][0] * mat[2][2] - mat[2][0] * mat[0][2],
                              mat[1][0] * mat[0][2] - mat[0][0] * mat[1][2]),
          VectorND<3, T, ISE>(
              mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1],
              mat[2][0] * mat[0][1] - mat[0][0] * mat[2][1],
              mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1]));
}

template <typename T, InstSetExt ISE>
T Determinant(const MatrixND<4, T, ISE> &m) {
  // This function is adopted from GLM
  /*
  ================================================================================
  OpenGL Mathematics (GLM)
  --------------------------------------------------------------------------------
  GLM is licensed under The Happy Bunny License and MIT License
  ================================================================================
  The Happy Bunny License (Modified MIT License)
  --------------------------------------------------------------------------------
  Copyright (c) 2005 - 2014 G-Truc Creation
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  Restrictions:
   By making use of the Software for military purposes, you choose to make a
   Bunny unhappy.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  ================================================================================
  The MIT License
  --------------------------------------------------------------------------------
  Copyright (c) 2005 - 2014 G-Truc Creation
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
   */

  T Coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
  T Coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
  T Coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

  T Coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
  T Coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
  T Coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

  T Coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
  T Coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
  T Coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

  T Coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
  T Coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
  T Coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

  T Coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
  T Coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
  T Coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

  T Coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
  T Coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
  T Coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

  using Vector = VectorND<4, T, ISE>;

  Vector Fac0(Coef00, Coef00, Coef02, Coef03);
  Vector Fac1(Coef04, Coef04, Coef06, Coef07);
  Vector Fac2(Coef08, Coef08, Coef10, Coef11);
  Vector Fac3(Coef12, Coef12, Coef14, Coef15);
  Vector Fac4(Coef16, Coef16, Coef18, Coef19);
  Vector Fac5(Coef20, Coef20, Coef22, Coef23);

  Vector Vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
  Vector Vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
  Vector Vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
  Vector Vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

  Vector Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
  Vector Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
  Vector Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
  Vector Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

  Vector SignA(+1, -1, +1, -1);
  Vector SignB(-1, +1, -1, +1);
  MatrixND<4, T, ISE> Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA,
                              Inv3 * SignB);

  Vector Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

  Vector Dot0(m[0] * Row0);
  T Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

  return Dot1;
}

template <typename T, InstSetExt ISE>
MatrixND<4, T, ISE> Inverse(const MatrixND<4, T, ISE> &m) {
  // This function is copied from GLM
  /*
  ================================================================================
  OpenGL Mathematics (GLM)
  --------------------------------------------------------------------------------
  GLM is licensed under The Happy Bunny License and MIT License
  ================================================================================
  The Happy Bunny License (Modified MIT License)
  --------------------------------------------------------------------------------
  Copyright (c) 2005 - 2014 G-Truc Creation
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  Restrictions:
   By making use of the Software for military purposes, you choose to make a
   Bunny unhappy.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  ================================================================================
  The MIT License
  --------------------------------------------------------------------------------
  Copyright (c) 2005 - 2014 G-Truc Creation
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
   */

  T Coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
  T Coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
  T Coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

  T Coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
  T Coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
  T Coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

  T Coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
  T Coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
  T Coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

  T Coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
  T Coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
  T Coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

  T Coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
  T Coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
  T Coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

  T Coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
  T Coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
  T Coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

  using Vector = VectorND<4, T, ISE>;

  Vector Fac0(Coef00, Coef00, Coef02, Coef03);
  Vector Fac1(Coef04, Coef04, Coef06, Coef07);
  Vector Fac2(Coef08, Coef08, Coef10, Coef11);
  Vector Fac3(Coef12, Coef12, Coef14, Coef15);
  Vector Fac4(Coef16, Coef16, Coef18, Coef19);
  Vector Fac5(Coef20, Coef20, Coef22, Coef23);

  Vector Vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
  Vector Vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
  Vector Vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
  Vector Vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

  Vector Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
  Vector Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
  Vector Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
  Vector Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

  Vector SignA(+1, -1, +1, -1);
  Vector SignB(-1, +1, -1, +1);
  MatrixND<4, T, ISE> Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA,
                              Inv3 * SignB);

  Vector Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

  Vector Dot0(m[0] * Row0);
  T Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

  T OneOverDeterminant = static_cast<T>(1) / Dot1;

  return Inverse * OneOverDeterminant;
}

template <int dim, typename T, InstSetExt ISE>
MIN_FORCE_INLINE MatrixND<dim, T, ISE> Inverse(const MatrixND<dim, T, ISE> &m) {
  return Inverse(m);
}

MIN_FORCE_INLINE Vector3 MultiplyMatrix4(const Matrix4 &m,
                                         const Vector3 &v,
                                         Float w) {
  return Vector3(m * Vector4(v, w));
}

}


