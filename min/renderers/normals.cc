#include "sample.h"
#include <min/visual/sampling.h>

namespace min {

class NormalsIntegrator : public SampleRenderer {
 public:
  virtual Spectrum Li(const Ray &ray, const std::shared_ptr<Scene> &scene, Sampler &sampler) {
    SurfaceIntersection isect;
    if (!scene->Intersect(ray, isect))
      return Spectrum(0.0f);
    return Spectrum(Abs(isect.shading_frame.n));
  }
};
MIN_IMPLEMENTATION(Renderer, NormalsIntegrator, "normals")

}

