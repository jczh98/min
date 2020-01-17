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
#include <nlohmann/json.hpp>
#include "defs.h"

namespace nlohmann {

template <int N, typename T, glm::qualifier Q>
struct adl_serializer<glm::vec<N, T, Q>> {
  using VecT = glm::vec<N, T, Q>;
  static void to_json(json &j, const VecT &v) {
    std::array<T, N> a;
    for (int i = 0; i < N; i++) {
      a[i] = static_cast<json::number_float_t>(v[i]);
    }
    j = std::move(a);
  }
  static void from_json(const json &j, VecT &v) {
    if (!j.is_array()) {
      MIN_ERROR(
          "Invalid JSON type [expected='array', actual='{}']", j.type_name());
    }
    if (j.size() != N) {
      MIN_ERROR(
          "Invalid number of elements [expected={}, actual={}]", N, j.size());
    }
    for (int i = 0; i < N; i++) {
      v[i] = static_cast<T>(j[i]);
    }
  }
};
}  // namespace nlohmann

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

using namespace glm;

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

template <class T>
struct Angle {
  Angle() = default;

  Angle(const T &v) : data(v) {}

  operator T() const { return data; }

  auto &get() const { return data; }

 private:
  T data;
};

template <class Value>
class TTransform {
  using mat4 = tmat4x4<Value>;
  using mat3 = tmat3x3<Value>;
  mat4 T, invT;
  mat3 T3, invT3, invT3T;

 public:
  TTransform() = default;

  explicit TTransform(const mat4 &T) : T(T), invT(glm::inverse(T)) {
    T3 = mat3(T);
    invT3 = mat3(invT);
    invT3T = transpose(invT3);
  }

  TTransform(const mat4 &T, const mat4 &invT) : T(T), invT(invT) {
    T3 = mat3(T);
    invT3 = mat3(invT);
    invT3T = transpose(invT3);
  }

  const mat4 &matrix() const { return T; }

  TTransform inverse() const {
    return TTransform{invT, T};
  }

  Vector3 TransformVector3(const Vector3 &v) const {
    return T3 * v;
  }

  Point3 TransformPoint3(const Point3 &v) const {
    auto x = T * Vector4(v.x, v.y, v.z, 1.0f);
    if (x.w == 1) {
      return Vector3(x);
    }
    return Vector3(x) / x.w;
  }

  Normal3 TransformNormal3(const Normal3 &v) const {
    return invT3T * v;
  }
};
using Transform = TTransform<Float>;
class TransformManipulator {
 public:
  Angle<Vector3> rotation;
  Vector3 translation;

  Transform ToTransform() const {
    Matrix4 m = Matrix4(1);
    m = rotate(m, rotation.get().z, Vector3(0, 0, 1));
    m = rotate(m, rotation.get().y, Vector3(1, 0, 0));
    m = rotate(m, rotation.get().x, Vector3(0, 1, 0));
    m = translate(m, translation);
    return Transform(m);
  }
};

template <class T>
void to_json(json &j, const Angle<T> &v) {
  j = {
      {"rad", v.get()}};
}

template <class T>
void from_json(const json &j, Angle<T> &v) {
  if (j.contains("deg")) {
    v = DegreesToRadians(j.at("deg").get<T>());
  } else if (j.contains("rad")) {
    v = j.at("rad").get<T>();
  } else {
    v = j.get<T>();
  }
}

inline void to_json(json &j, const TransformManipulator &transform) {
  j = json::object();
  j["rotation"] = transform.rotation;
  j["translation"] = transform.translation;
}

inline void from_json(const json &j, TransformManipulator &transform) {
  transform.rotation = j.at("rotation").get<Angle<Vector3>>();
  transform.translation = j.at("translation").get<Vector3>();
}

}  // namespace min::ray