#pragma once

#include <min-ray/filter.h>
#include <min-ray/json.h>

namespace min::ray {

class TentFilter : public ReconstructionFilter {
 public:
  TentFilter() {
    radius = 1.0f;
  }

  void initialize(const Json &json) override {
    radius = rjson::GetOrDefault(json, "radius", 1.0f);
  }

  float Evaluate(float x) const {
    return std::max(0.0f, 1.0f - std::abs(x));
  }

  std::string ToString() const {
    return "TentFilter[]";
  }
};
MIN_IMPLEMENTATION(ReconstructionFilter, TentFilter, "tent")

}

