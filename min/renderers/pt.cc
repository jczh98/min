#include "sample.h"
#include <min/visual/distribution.h>
#include <min/visual/sampling.h>

namespace min {

class PathTracer : public SampleRenderer {

  Spectrum SpecularReflect(const Ray &ray, const SurfaceIntersection &isect,
                           const std::shared_ptr<Scene> &scene, Sampler &sampler, int depth) const {
    Vector3 wo = isect.wo, wi;
    Float pdf;
    wo = isect.ToLocal(wo);
    BSDFSample bsdf_sample;
    bsdf_sample.wo = wo;
    isect.bsdf->Sample(sampler.Get2D(), isect.sp, bsdf_sample);
    if (bsdf_sample.sampled_type != BSDF::Type(BSDF::Type::kReflection | BSDF::Type::kSpecular)) {
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

  Spectrum UniformSampleOneLight(const SurfaceIntersection &it, const std::shared_ptr<Scene> &scene,
      Sampler &sampler, const Distribution1D *light_distrib = nullptr) const {
    int n_lights = int(scene->lights.size());
    if (n_lights == 0) return Spectrum(0);
    int light_num;
    Float light_pdf;
    if (light_distrib) {
      light_num = light_distrib->SampleDiscrete(sampler.Get1D(), &light_pdf);
      if (light_pdf == 0) return Spectrum(0);
    } else {
      light_num = std::min((int)(sampler.Get1D() * n_lights), n_lights - 1);
      light_pdf = 1.0 / n_lights;
    }
    const std::shared_ptr<Light> &light = scene->lights[light_num];
    Point2f u_light = sampler.Get2D();
    Point2f u_scattering = sampler.Get2D();
    return EstimateDirect(scene, it, u_scattering, u_light, *light, sampler) / light_pdf;
  }

  Spectrum EstimateDirect(const std::shared_ptr<Scene> &scene, const SurfaceIntersection &it,
      const Point2f u_scattering, const Point2f u_light,
      const Light &light,Sampler &sampler, bool specular = false) const {
    BSDF::Type bsdf_type = specular ? BSDF::Type::kAll : BSDF::Type::kAllButSpecular;
    Spectrum Ld(0);
    LightSample light_sample;
    VisibilityTester tester;
    light.SampleLi(u_light, it, light_sample, tester);
    Spectrum Li = light_sample.li;
    Float scattering_pdf = 0;
    if (light_sample.pdf > 0 && !IsBlack(Li)) {
      // Compute BSDF
      Spectrum f;
      f = it.bsdf->Evaluate(it.sp, it.ToLocal(it.wo), it.ToLocal(light_sample.wi)) * AbsDot(light_sample.wi, it.shading_frame.n);
      scattering_pdf = it.bsdf->EvaluatePdf(it.sp, it.ToLocal(it.wo), it.ToLocal(light_sample.wi));
      if (!IsBlack(f)) {
        if (!tester.Unoccluded(scene)) {
          Li = Spectrum(0);
        }
        if (!IsBlack(Li)) {
          if (IsDeltaLight(light.flags)) {
            Ld += f * Li / light_sample.pdf;
          } else {
            Float weight = PowerHeuristic(1, light_sample.pdf, 1, scattering_pdf);
            Ld += f * Li * weight / light_sample.pdf;
          }
        }
      }
    }

    // Sample BSDF with MIS
    if (!IsDeltaLight(light.flags)) {
      Spectrum f;
      bool sampled_specular = false;
      BSDFSample bsdf_sample;
      bsdf_sample.wo = it.ToLocal(it.wo);
      it.bsdf->Sample(u_scattering, it.sp, bsdf_sample);
      f = bsdf_sample.f * AbsDot(it.ToWorld(bsdf_sample.wi), it.shading_frame.n);
      sampled_specular = (bsdf_sample.sampled_type & BSDF::Type::kSpecular) != 0;
      if (!IsBlack(f) && bsdf_sample.pdf > 0) {
        Float weight = 1;
        if (!sampled_specular) {
          Float light_pdf = light.PdfLi(it, it.ToWorld(bsdf_sample.wi));
          if (light_pdf == 0) return Ld;
          weight = PowerHeuristic(1, bsdf_sample.pdf, 1, light_pdf);
        }
        SurfaceIntersection light_isect;
        Ray ray = it.SpawnRay(it.ToWorld(bsdf_sample.wi));
        bool found_intersection = scene->Intersect(ray, light_isect);
        Spectrum Li(0);
        if (found_intersection) {
          if (light_isect.shape->area_light.get() == &light) {
            Li = light.L(light_isect, -it.ToWorld(bsdf_sample.wi));
          }
        } else {
          Li = light.Le(ray);
        }
        if (!IsBlack(Li)) Ld += f * Li * weight / bsdf_sample.pdf;
      }
    }
    return Ld;
  }
 public:
  Spectrum Li(const Ray &ray, const std::shared_ptr<Scene> &scene, Sampler &sampler) override {
    return Li(ray, scene, sampler, 0);
  }

  Spectrum Li(const Ray &ray, const std::shared_ptr<Scene> &scene, Sampler &sampler, int depth) const {
    Spectrum L(0);
    SurfaceIntersection isect;
    if (!scene->Intersect(ray, isect)) {
      for (const auto &light : scene->lights) L += light->Le(ray);
      return L;
    }
    if (!isect.bsdf) {
      return Li(isect.SpawnRay(ray.d), scene, sampler, depth);
    }
    Vector3 wo = isect.wo;
    if (isect.shape->area_light) {
      L += isect.shape->area_light->L(isect, wo);
    }
    if (scene->lights.size() > 0) {
      L += UniformSampleOneLight(isect, scene, sampler);
    }
    if (depth + 1 < 5) {
      L += SpecularReflect(ray, isect, scene, sampler, depth);
    }
    return L;
  }
};
MIN_IMPLEMENTATION(Renderer, PathTracer, "pt")

}

