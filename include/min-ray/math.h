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

#include <Eigen/Core>
#include "defs.h"

namespace min::ray {

using Vector3 = Eigen::Vector3<Float>;
using Vector2 = Eigen::Vector2<Float>;
using Vector4 = Eigen::Vector4<Float>;
using Vector2i = Eigen::Vector2i;
using Matrix3 = Eigen::Matrix3<Float>;
using Matrix4 = Eigen::Matrix4<Float>;
using Point3 = Vector3;
using Point2 = Vector2;
using Point2i = Vector2i;
using Normal3 = Vector3;

inline void ComputeLocalFrame(const Vector3 &v1, Vector3 *v2, Vector3 *v3) {
  if (std::abs(v1.x()) > std::abs(v1.y()))
    *v2 = Vector3(-v1.z(), 0, v1.x()) / std::sqrt(v1.x() * v1.x() + v1.z() * v1.z());
  else
    *v2 = Vector3(0, v1.z(), -v1.y()) / std::sqrt(v1.y() * v1.y() + v1.z() * v1.z());
  *v3 = v1.cross(*v2).normalized();
}

struct CoordinateSystem {
  CoordinateSystem() = default;

  explicit CoordinateSystem(const Vector3 &v) : normal(v) { ComputeLocalFrame(v, &localX, &localZ); }

  Vector3 WorldToLocal(const Vector3 &v) const {
    return Vector3(localX.dot(v), normal.dot(v), localZ.dot(v));
  }

  Vector3 LocalToWorld(const Vector3 &v) const {
    return v.x() * localX + v.y() * normal + v.z() * localZ;
  }

 private:
  Vector3 normal;
  Vector3 localX, localZ;
};

template <int N, class T>
class BoundingBox {
 public:
  using Vec = Eigen::Vector<T, N>
      BoundingBox UnionOf(cosnt BoundingBox &box) const {
    return BoundingBox(min(pmin, box.pmin), max(pmax, box.pmax);)
  }
  Vec Centroid() const { return (pmin + pmax) * 0.5; }
  Vec Size() const { return pmax - pmin; }
  T Area() {
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

  Vec pmin, pmax;
};

using BoundingBox3 = BoundingBox<3, Float>;
}  // namespace min::ray