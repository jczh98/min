#include "sample.h"
#include <min/visual/sampling.h>

namespace min {

class AOIntegrator : public SampleRenderer {
  int n_samples;
 public:
  void initialize(const Json &json) override {
    SampleRenderer::initialize(json);
    n_samples = Value(json, "n_samples", 64);
  }
  virtual Spectrum Li(const Ray &ray, const std::shared_ptr<Scene> &scene, Sampler &sampler) {
    SurfaceIntersection isect;
    Spectrum L(0);
    if (scene->Intersect(ray, isect)) {
      for (int i = 0; i < n_samples; i++) {
        Vector3f wi = CosineSampleHemisphere(sampler.Get2D());
        Float pdf = std::abs(wi.z) * kInvPi;
        wi = isect.ToWorld(wi);
        if (!scene->IntersectP(isect.SpawnRay(wi))) {
          L += Dot(wi, isect.shading_frame.n) / (pdf * n_samples);
        }
       }
    }
    return L;
  }
};
MIN_IMPLEMENTATION(Renderer, AOIntegrator, "ao")

}



