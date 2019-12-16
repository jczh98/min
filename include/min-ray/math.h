// MIT License
//
// Copyright (c) 2018-2019, neverfelly <neverfelly@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "defs.h"

namespace min::ray {

using Vector3 = glm::tvec3<Float>;
using Vector2 = glm::tvec2<Float>;
using Vector4 = glm::tvec4<Float>;
using Vector2i = glm::ivec2;
using Vector3i = glm::ivec3;
using Matrix3 = glm::tmat3x3<Float>;
using Matrix4 = glm::tmat4x4<Float>;
using Point3 = Vector3;
using Point2 = Vector2;
using Point2i = Vector2i;
using Point3i = Vector3i;
using Normal3 = Vector3;

inline void ComputeLocalFrame(const Vector3 &v1, Vector3 *v2, Vector3 *v3) {
  if (std::abs(v1.x) > std::abs(v1.y))
    *v2 = Vector3(-v1.z, 0, v1.x) / std::sqrt(v1.x * v1.x + v1.z * v1.z);
  else
    *v2 = Vector3(0, v1.z, -v1.y) / std::sqrt(v1.y * v1.y + v1.z * v1.z);
  *v3 = glm::normalize(glm::cross(v1, *v2));
}

struct CoordinateSystem {
  CoordinateSystem() = default;

  explicit CoordinateSystem(const Vector3 &v) : normal(v) { ComputeLocalFrame(v, &localX, &localZ); }

  Vector3 WorldToLocal(const Vector3 &v) const {
    return Vector3(glm::dot(localX, v), glm::dot(normal, v), glm::dot(localZ, v));
  }

  Vector3 LocalToWorld(const Vector3 &v) const {
    return v.x * localX + v.y * normal + v.z * localZ;
  }

 private:
  Vector3 normal;
  Vector3 localX, localZ;
};

template <int N, class T, glm::qualifier Q>
class BoundingBox {
 public:
  BoundingBox UnionOf(const BoundingBox &box) const { return BoundingBox{glm::min(pmin, box.pmin), glm::max(pmax, box.pmax)}; }

  BoundingBox UnionOf(const glm::vec<N, T, Q> &rhs) const { return BoundingBox{glm::min(pmin, rhs), glm::max(pmax, rhs)}; }

  glm::vec<N, T, Q> Centroid() const { return (pmin + pmax) * 0.5f; }

  glm::vec<N, T, Q> Size() const { return pmax - pmin; }

  T Area() const {
    auto a = (Size()[0] * Size()[1] + Size()[0] * Size()[2] + Size()[1] * Size()[2]);
    return a + a;
  }

  bool Intersect(const BoundingBox &rhs) const {
    for (size_t i = 0; i < N; i++) {
      if (pmin[i] > rhs.pmax[i] || pmax[i] < rhs.pmin[i])
        continue;
      else {
        return true;
      }
    }
    return false;
  }

  glm::vec<N, T, Q> offset(const glm::vec<N, T, Q> &p) const {
    auto o = p - pmin;
    return o / Size();
  }

  glm::vec<N, T, Q> pmin, pmax;
};

using BoundingBox3 = BoundingBox<3, Float, glm::qualifier::defaultp>;

template <class T>
T Lerp3(const T &v1, const T &v2, const T &v3, float u, float v) {
  return (1 - u - v) * v1 + u * v2 + v * v3;
}

template <int N, typename T, glm::qualifier Q>
T MinComp(const glm::vec<N, T, Q> &v) {
  auto x = v[0];
  for (int i = 1; i < N; i++) {
    x = glm::min(x, v[i]);
  }
  return x;
}

template <int N, typename T, glm::qualifier Q>
T MaxComp(const glm::vec<N, T, Q> &v) {
  auto x = v[0];
  for (int i = 1; i < N; i++) {
    x = glm::max(x, v[i]);
  }
  return x;
}

template <class T>
T Lerp(const T &x, const T &y, const T &a) {
  return x * T(1.0f - a) + y * a;
}

template <class T>
struct Radians;

template <class T>
struct Degrees {
  Degrees() = default;
  explicit Degrees(T v) : value(v) {}

  explicit operator T() const {
    return value;
  }

  inline explicit Degrees(const Radians<T> &);

  T &get() { return value; }

  const T &get() const { return value; }

 private:
  T value = T();
};

template <class T>
struct Radians {
  Radians() = default;
  explicit Radians(T v) : value(v) {}

  explicit Radians(const Degrees<T> &degrees) {
    value = DegreesToRadians(T(degrees));
  }

  explicit operator T() const {
    return value;
  }

  T &get() { return value; }

  [[nodiscard]] const T &get() const { return value; }

  private : T value = T();
};

class Transform {
 public:
  Transform() = default;
  explicit Transform(const Matrix4 &mat) : mat4_(mat), inv_mat4_(glm::inverse(mat4_)) {
    mat3_ = Matrix3(mat4_);
    inv_mat3_ = Matrix3(inv_mat4_);
    inv_mat3_t_ = glm::transpose(inv_mat3_);
  }

  Transform(const Matrix4 &mat, const Matrix4 &inv) : mat4_(mat), inv_mat4_(inv) {
    mat3_ = Matrix3(mat4_);
    inv_mat3_ = Matrix3(inv_mat4_);
    inv_mat3_t_ = glm::transpose(inv_mat3_);
  }

  const Matrix4 &Matrix() const { return mat4_; }
  Transform Inverse() const {
    return Transform(inv_mat4_, mat4_);
  }

  Vector3 TransformVector3(const Vector3 &v) const {
    return mat3_ * v;
  }

  Vector3 TransformPoint3(const Vector3 &v) const {
    auto x = mat4_ * Vector4(v, 1);
    if (x.w == 1) {
      return x;
    }
    return Vector3(x) / x.w;
  }

 private:
  Matrix4 mat4_, inv_mat4_;
  Matrix3 mat3_, inv_mat3_, inv_mat3_t_;
};

class TransformManipulator {
 public:
  Radians<Vector3> rotation;
  Vector3 translation;

  Transform toTransform() const {
    Matrix4 m = glm::identity<Matrix4>();
    m = glm::rotate(rotation.get().z, Vector3(0, 0, 1)) * m;
    m = glm::rotate(rotation.get().y, Vector3(1, 0, 0)) * m;
    m = glm::rotate(rotation.get().x, Vector3(0, 1, 0)) * m;
    m = glm::translate(translation) * m;
    return Transform(m);
  }
};

template <class T>
inline Degrees<T>::Degrees(const Radians<T> &r) {
  value = RadiansToDegrees(T(r));
}

}  // namespace min::ray