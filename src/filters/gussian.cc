#pragma once

#include <min-ray/filter.h>

namespace min::ray {

class GaussianFilter : public ReconstructionFilter {
 public:
  GaussianFilter() {
    radius = 2.0f;
    stddev = 0.5f;
  }
  void initialize(const json &json) override {
    radius = json.at("radius").get<float>();
    stddev = json.at("stddev").get<float>();
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