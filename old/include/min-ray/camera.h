#pragma once

#include <min-ray/interface.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>

namespace min::ray {

class Camera : public Unit {
 public:

  virtual Color3f SampleRay(Ray3f &ray,
                            const Point2f &samplePosition,
                            const Point2f &apertureSample) const = 0;

  const Vector2i &GetOutputSize() const { return output_size; }

  Vector2i output_size;
  std::shared_ptr<ReconstructionFilter> filter;
};
MIN_INTERFACE(Camera)

}  // namespace min::ray
