

#pragma once

#include <min-ray/interface.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>

namespace min::ray {

/**
 * \brief Abstract integrator (i.e. a rendering technique)
 *
 * In Nori, the different rendering techniques are collectively referred to as 
 * integrators, since they perform integration over a high-dimensional
 * space. Each integrator represents a specific approach for solving
 * the light transport equation---usually favored in certain scenarios, but
 * at the same time affected by its own set of intrinsic limitations.
 */
class Integrator : public Unit {
 public:
  /// Release all memory
  virtual ~Integrator() {}

  /// Perform an (optional) preprocess step
  virtual void Preprocess(const Scene *scene) {}

  /**
     * \brief Sample the incident radiance along a ray
     *
     * \param scene
     *    A pointer to the underlying scene
     * \param sampler
     *    A pointer to a sample generator
     * \param ray
     *    The ray in question
     * \return
     *    A (usually) unbiased estimate of the radiance in this direction
     */
  virtual Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const = 0;

};
MIN_INTERFACE(Integrator)

}  // namespace min::ray
