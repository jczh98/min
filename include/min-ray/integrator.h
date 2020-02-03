

#pragma once

#include <min-ray/interface.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>

namespace min::ray {

class Integrator : public Unit {
 public:

  virtual ~Integrator() {}

  virtual void Preprocess(const Scene *scene) {}

  virtual Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const = 0;

};
MIN_INTERFACE(Integrator)

}  // namespace min::ray
