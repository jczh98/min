#pragma once

#include <min/common/json_type.h>
#include <min/math/math.h>

namespace min {

inline Json AtOrEmpty(Json j, std::string key) {
  if (j.contains(key)) {
    return j.at(key);
  } else {
    return {};
  }
}

inline Json GetProps(Json j) {
  return AtOrEmpty(j, "props");
}

template <typename T>
T Value(const Json &json, const std::string &key, const T &default_value) {
  if (json.find(key) != json.end()) {
    return json.at(key).get<T>();
  }
  return default_value;
}

}

namespace nlohmann {

using namespace min;

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


}