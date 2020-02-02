#pragma once

#include "common.h"
#include "vector.h"
#include "transform.h"
#include "color.h"
#include <Eigen/Geometry>
#include <nlohmann/json.hpp>

namespace nlohmann {

using namespace min::ray;

template<typename T, int N>
struct adl_serializer<TVector<T, N>> {
  using VecT = TVector<T, N>;

  static void to_json(json &j, const VecT &v) {
    std::array<T, N> a;
    for (int i = 0; i < N; i++) {
      a[i] = static_cast<json::number_float_t>(v[i]);
    }
    j = std::move(a);
  }
  static void from_json(const json &j, VecT &v) {
    if (!j.is_array()) {
      MIN_ERROR("Invalid JSON type [expected='array', actual='{}']", j.type_name());
    }
    if (j.size() != N) {
      MIN_ERROR("Invalid number of elements [expected={}, actual={}]", N, j.size());
    }
    for (int i = 0; i < N; i++) {
      v[i] = static_cast<T>(j[i]);
    }
  }
};

template<typename T, int N>
struct adl_serializer<TPoint<T, N>> {
  using PointT = TPoint<T, N>;

  static void to_json(json &j, const PointT &v) {
    std::array<T, N> a;
    for (int i = 0; i < N; i++) {
      a[i] = static_cast<json::number_float_t>(v[i]);
    }
    j = std::move(a);
  }
  static void from_json(const json &j, PointT  &v) {
    if (!j.is_array()) {
      MIN_ERROR("Invalid JSON type [expected='array', actual='{}']", j.type_name());
    }
    if (j.size() != N) {
      MIN_ERROR("Invalid number of elements [expected={}, actual={}]", N, j.size());
    }
    for (int i = 0; i < N; i++) {
      v[i] = static_cast<T>(j[i]);
    }
  }
};

template <>
struct adl_serializer<Color3f> {

  static void to_json(json &j, const Color3f &v) {
    std::array<float, 3> a;
    for (int i = 0; i < 3; i++) {
      a[i] = static_cast<json::number_float_t>(v[i]);
    }
    j = std::move(a);
  }
  static void from_json(const json &j, Color3f  &v) {
    if (!j.is_array()) {
      MIN_ERROR("Invalid JSON type [expected='array', actual='{}']", j.type_name());
    }
    if (j.size() != 3) {
      MIN_ERROR("Invalid number of elements [expected={}, actual={}]", 3, j.size());
    }
    for (int i = 0; i < 3; i++) {
      v[i] = static_cast<float>(j[i]);
    }
  }
};

template <>
struct adl_serializer<Color4f> {

  static void to_json(json &j, const Color4f &v) {
    std::array<float, 4> a;
    for (int i = 0; i < 4; i++) {
      a[i] = static_cast<json::number_float_t>(v[i]);
    }
    j = std::move(a);
  }
  static void from_json(const json &j, Color4f  &v) {
    if (!j.is_array()) {
      MIN_ERROR("Invalid JSON type [expected='array', actual='{}']", j.type_name());
    }
    if (j.size() != 3) {
      MIN_ERROR("Invalid number of elements [expected={}, actual={}]", 4, j.size());
    }
    for (int i = 0; i < 4; i++) {
      v[i] = static_cast<float>(j[i]);
    }
  }
};

template <>
struct adl_serializer<Transform> {
  static void from_json(const json& j, Transform &v) {
    Eigen::Affine3f transform;
    transform.setIdentity();
    if (j.contains("scale")) {
      auto vec = j.at("scale").get<Vector3f>();
      transform = Eigen::DiagonalMatrix<float, 3>(vec) * transform;
    }
    if (j.contains("lookat")) {
      auto target = j.at("lookat").at("target").get<Vector3f>();
      auto origin = j.at("lookat").at("origin").get<Vector3f>();
      auto up = j.at("lookat").at("up").get<Vector3f>();
      Vector3f dir = (target - origin).normalized();
      Vector3f left = up.normalized().cross(dir).normalized();
      Vector3f newUp = dir.cross(left).normalized();

      Eigen::Matrix4f trafo;
      trafo << left, newUp, dir, origin,
          0, 0, 0, 1;

      transform = Eigen::Affine3f(trafo) * transform;
    }
    v = transform.matrix();
  }
};

}