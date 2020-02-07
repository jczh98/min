#pragma once

#include <min-ray/json_type.h>

namespace min::ray {

template <typename T>
inline T GetOrDefault(Json j, std::string key, T default_val) {
  if (j.contains(key)) {
    return j.at(key).get<T>();
  } else {
    return default_val;
  }
}

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

}