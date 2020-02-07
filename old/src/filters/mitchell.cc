#pragma once

#include <min-ray/filter.h>
#include <min-ray/json.h>

namespace min::ray {

class MitchellNetravaliFilter : public ReconstructionFilter {
 public:

  void initialize(const Json &json) override {
    radius = rjson::GetOrDefault(json, "radius", 2.0f);
    B = rjson::GetOrDefault(json, "B", 1.0f / 3.0f);
    C = rjson::GetOrDefault(json, "C", 1.0f / 3.0f);
  }

  float Evaluate(float x) const {
    x = std::abs(2.0f * x / radius);
    float x2 = x * x, x3 = x2 * x;

    if (x < 1) {
      return 1.0f / 6.0f * ((12 - 9 * B - 6 * C) * x3 + (-18 + 12 * B + 6 * C) * x2 + (6 - 2 * B));
    } else if (x < 2) {
      return 1.0f / 6.0f * ((-B - 6 * C) * x3 + (6 * B + 30 * C) * x2 + (-12 * B - 48 * C) * x + (8 * B + 24 * C));
    } else {
      return 0.0f;
    }
  }

  std::string ToString() const {
    return fmt::format("MitchellNetravaliFilter[radius={}, B={}, C={}]", radius, B, C);
  }

 protected:
  float B, C;
};
MIN_IMPLEMENTATION(ReconstructionFilter, MitchellNetravaliFilter, "mitchell")

}