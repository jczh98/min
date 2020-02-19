#include "sample.h"

namespace min {

class Whitted : public SampleRenderer {

  Spectrum SpecularReflect(const Ray &ray, const SurfaceIntersection &isect,
                           const std::shared_ptr<Scene> &scene, Sampler &sampler, int depth) const {
    Vector3 wo = isect.wo, wi;
    Float pdf;
    wo = isect.ToLocal(wo);
    BSDFSample bsdf_sample;
    bsdf_sample.wo = wo;
    isect.bsdf->Sample(sampler.Get2D(), isect.sp, bsdf_sample);
    if (bsdf_sample.sampled_type == BSDF::Type(BSDF::Type::kReflection | BSDF::Type::kSpecular)) {
      return Spectrum(0);
    }
    pdf = bsdf_sample.pdf;
    wi = isect.ToWorld(bsdf_sample.wi);
    Spectrum f = bsdf_sample.f;
    const Normal3f &ns = isect.shading_frame.n;
    if (pdf > 0 && !IsBlack(f) && AbsDot(wi, ns) != 0.0f) {
      Ray rd = isect.SpawnRay(wi);
      return f * Li(rd, scene, sampler, depth + 1) * AbsDot(wi, ns) / pdf;
    } else {
      return Spectrum(0);
    }
  }

 public:
  Spectrum Li(const Ray &ray, const std::shared_ptr<Scene> &scene, Sampler &sampler) override {
    return Li(ray, scene, sampler, 0);
  }

  Spectrum Li(const Ray &ray, const std::shared_ptr<Scene> &scene, Sampler &sampler, int depth)  const {
    Spectrum L(0);
    SurfaceIntersection isect;
    if (!scene->Intersect(ray, isect)) {
      return L;
    }
    const Normal3f &n = isect.shading_frame.n;
    Vector3f wo = isect.wo;
    if (!isect.bsdf) {
      return L;
    }
    if (isect.shape->area_light) {
      L += isect.shape->area_light->L(isect, wo);
    }
    for (const auto &light : scene->lights) {
      LightSample light_sample;
      Vector3 wi;
      Float pdf;
      VisibilityTester vis;
      light->SampleLi(sampler.Get2D(), isect, light_sample, vis);
      wi = light_sample.wi;
      pdf = light_sample.pdf;
      Spectrum Li = light_sample.li;
      if (IsBlack(Li) || pdf == 0) continue;
      Spectrum f = isect.bsdf->Evaluate(isect.sp, isect.ToLocal(wo), isect.ToLocal(wi));
      if (!IsBlack(f) && vis.Unoccluded(scene)) {
        L += f * Li * AbsDot(wi, n) / pdf;
      }
    }
    if (depth + 1 < 5) {
      L += SpecularReflect(ray, isect, scene, sampler, depth);
    }
    return L;
  }
};
MIN_IMPLEMENTATION(Renderer, Whitted, "whitted")

}

