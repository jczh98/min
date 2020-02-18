#include "sample.h"

namespace min {

class PathTracer : public SampleRenderer {
 public:
  Spectrum Li(const Ray &ray, const std::shared_ptr<Scene> &scene, Sampler &sampler) override {
    return min::Spectrum(0);
  }
};
MIN_IMPLEMENTATION(Renderer, PathTracer, "pt")

}

