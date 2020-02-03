

#pragma once

#include <min-ray/interface.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>

namespace min::ray {

class Integrator : public Unit {
 public:

  virtual void Preprocess(const std::shared_ptr<Scene> scene) {}

  virtual Color3f Li(const std::shared_ptr<Scene> scene,
      Sampler *sampler, const Ray3f &ray) const = 0;

};
MIN_INTERFACE(Integrator)

}  // namespace min::ray
