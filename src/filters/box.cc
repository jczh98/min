#pragma once

#include <min-ray/filter.h>

namespace min::ray {

class BoxFilter : public ReconstructionFilter {
 public:
  BoxFilter() {
    radius = 0.5f;
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