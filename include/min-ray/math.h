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
using Matrix3 = glm::tmat3x3<Float>;
using Matrix4 = glm::tmat4x4<Float>;
using Point3 = Vector3;
using Point2 = Vector2;
using Point2i = Vector2i;
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
  BoundingBox UnionOf(const BoundingBox &box) const { return BoundingBox{min(pmin, box.pmin), max(pmax, box.pmax)}; }

  BoundingBox UnionOf(const glm::vec<N, T, Q> &rhs) const { return BoundingBox{min(pmin, rhs), max(pmax, rhs)}; }

  glm::vec<N, T, Q> Centroid() const { return (pmin + pmax) * 0.5f; }

  glm::vec<N, T, Q> Size() const { return pmax - pmin; }

  T Area() const {
    auto a = (size()[0] * size()[1] + size()[0] * size()[2] + size()[1] * size()[2]);
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
    return mat3_ *v;
  }

 private:
  Matrix4 mat4_, inv_mat4_;
  Matrix3 mat3_, inv_mat3_, inv_mat3_t_;
};

class TransformManipulator 
{
  public:

};
}  // namespace min::ray