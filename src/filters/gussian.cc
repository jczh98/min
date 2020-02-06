#pragma once

#include <min-ray/filter.h>
#include <min-ray/json.h>

namespace min::ray {

class GaussianFilter : public ReconstructionFilter {
 public:

  void initialize(const Json &json) override {
    radius = rjson::GetOrDefault(json, "radius", 2.0f);
    stddev = rjson::GetOrDefault(json, "stddev", 0.5f);
  }

  float Evaluate(float x) const {
    float alpha = -1.0f / (2.0f * stddev * stddev);
    return std::max(0.0f,
                    std::exp(alpha * x * x) -
                        std::exp(alpha * radius * radius));
  }

 protected:
  float stddev;
};
MIN_IMPLEMENTATION(ReconstructionFilter, GaussianFilter, "gaussian")

}