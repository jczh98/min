#pragma once

#include <min-ray/interface.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>

// Reconstruction filters will be tabulated at this resolution
#define MIN_FILTER_RESOLUTION 32

namespace min::ray {

class ReconstructionFilter : public Unit {
 public:
  // Return the filter radius in fractional pixels
  float GetRadius() const { return radius; }

  // Evaluate the filter function
  virtual float Evaluate(float x) const = 0;

 protected:
  float radius;
};
MIN_INTERFACE(ReconstructionFilter)

}  // namespace min::ray
