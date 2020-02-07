#pragma once

#include <min-ray/filter.h>
#include <min-ray/json.h>

namespace min::ray {

class BoxFilter : public ReconstructionFilter {
 public:

  void initialize(const Json &json) override {
    radius = rjson::GetOrDefault(json, "radius", 0.5f);
  }

  float Evaluate(float) const {
    return 1.0f;
  }

  std::string ToString() const {
    return "BoxFilter[]";
  }
};
MIN_IMPLEMENTATION(ReconstructionFilter, BoxFilter, "box")

}