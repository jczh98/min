#include "sample.h"

namespace min {

class Whitted : public SampleRenderer {
 public:
  Spectrum Li(const Ray &ray, const std::shared_ptr<Scene> &scene, Sampler &sampler) override {
    return Li(ray, scene, sampler, 0);
  }

  Spectrum Li(const Ray &ray, const std::shared_ptr<Scene> &scene, Sampler &sampler, int depth)  {
    Spectrum L(0);
    SurfaceIntersection isect;
    if (!scene->Intersect(ray, isect)) {
      return L;
    }
    const Normal3f &n = isect.shading_frame.n;
    Vector3f wo = isect.wo;
    if (!isect.bsdf) {
      return Li(isect.SpawnRay(ray.d), scene, sampler, depth);
    }
    if (isect.shape->area_light) {
      L += isect.shape->area_light->L(isect, wo);
    }
    for (const auto &light : scene->lights) {
      LightSample light_sample;
      VisibilityTester vis;
      light->SampleLi(sampler.Get2D(), isect, light_sample, vis);
      Spectrum Li = light_sample.li;
      Float pdf = light_sample.pdf;
      if (IsBlack(Li) || pdf == 0) continue;
      Spectrum f = isect.bsdf->Evaluate(isect.sp, wo, light_sample.wi);
      if (!IsBlack(f) && vis.Unoccluded(scene)) {
        L += f * Li * AbsDot(light_sample.wi, n) / pdf;
      }
    }
    if (depth + 1 < 5) {
      BSDFSample bsdf_sample;
      bsdf_sample.wo = wo;
      isect.bsdf->Sample(sampler.Get2D(), isect.sp, bsdf_sample);
      Spectrum f = bsdf_sample.f;
      Normal3 ns = isect.shading_frame.n;
      if (bsdf_sample.pdf > 0.0f && !IsBlack(f) && AbsDot(bsdf_sample.wi, ns) != 0.0f) {
        L += bsdf_sample.f * Li(isect.SpawnRay(bsdf_sample.wi), scene, sampler, depth + 1) *
            AbsDot(bsdf_sample.wi, isect.shading_frame.n) / bsdf_sample.pdf;
      } else {
        L += Spectrum(0);
      }
    }
    return L;
  }
};
MIN_IMPLEMENTATION(Renderer, Whitted, "whitted")

}

