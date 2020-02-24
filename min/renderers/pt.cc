#include "sample.h"
#include <min/visual/distribution.h>
#include <min/visual/sampling.h>

namespace min {

class PathTracer : public SampleRenderer {

  int max_depth = 5;
  Float threshold = 1.0;

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
    BxDF::Type bsdf_flags = specular ? BxDF::Type::kAll : BxDF::Type::kAllButSpecular;
    Spectrum Ld(0);
    Vector3 wi;
    Float light_pdf = 0, scattering_pdf = 0;
    LightSample light_sample;
    VisibilityTester tester;
    light.SampleLi(u_light, it, light_sample, tester);
    wi = light_sample.wi;
    light_pdf = light_sample.pdf;
    Spectrum Li = light_sample.li;
    if (light_pdf > 0 && !IsBlack(Li)) {
      // Compute BSDF
      Spectrum f;
      f = it.bsdf->Evaluate(it.wo, wi, bsdf_flags)
          * AbsDot(wi, it.shading_frame.n);
      scattering_pdf = it.bsdf->Pdf(it.wo, wi, bsdf_flags);
      if (!IsBlack(f)) {
        if (!tester.Unoccluded(scene)) {
          Li = Spectrum(0);
        }
        // Light contribution
        if (!IsBlack(Li)) {
          if (IsDeltaLight(light.flags)) {
            Ld += f * Li / light_sample.pdf;
          } else {
            Float weight = PowerHeuristic(1, light_pdf, 1, scattering_pdf);
            Ld += f * Li* weight / light_pdf;
          }
        }
      }
    }

    // Sample BSDF with MIS
    if (!IsDeltaLight(light.flags)) {
      Spectrum f;
      bool sampled_specular = false;
      BSDFSample bsdf_sample;
      it.bsdf->Sample(u_scattering, it.wo, bsdf_sample, bsdf_flags);
      wi = bsdf_sample.wi;
      scattering_pdf = bsdf_sample.pdf;
      f = bsdf_sample.f * AbsDot(wi, it.shading_frame.n);
      sampled_specular = (bsdf_sample.sampled_type & BxDF::Type::kSpecular) != 0;
      if (!IsBlack(f) && scattering_pdf > 0) {
        Float weight = 1;
        if (!sampled_specular) {
          Float light_pdf = light.PdfLi(it, wi);
          if (light_pdf == 0) return Ld;
          weight = PowerHeuristic(1, scattering_pdf, 1, light_pdf);
        }
        SurfaceIntersection light_isect;
        Ray ray = it.SpawnRay(wi);
        bool found_intersection = scene->Intersect(ray, light_isect);
        Spectrum Li(0);
        if (found_intersection) {
          if (light_isect.shape->area_light.get() == &light) {
            Li = light.L(light_isect, -wi);
          }
        } else {
          Li = light.Le(ray);
        }
        if (!IsBlack(Li)) Ld += f * Li * weight / scattering_pdf;
      }
    }
    return Ld;
  }
 public:
  Spectrum Li(const Ray &r, const std::shared_ptr<Scene> &scene, Sampler &sampler) override {
    Spectrum L(0), beta(1);
    Ray ray(r);
    bool specular = false;
    int depth;
    Float eta_scale = 1.0f;
    for (depth = 0;;++depth) {
      SurfaceIntersection isect;
      bool found_intersection = scene->Intersect(ray, isect);
      if (depth == 0 || specular) {
        if (found_intersection) {
          if (isect.shape->area_light) {
            L += beta * isect.shape->area_light->L(isect, -ray.d);
          }
        } else {
          // Count background light
        }
      }
      if (!found_intersection || depth >= max_depth) break;
      isect.ComputeScatteringEvents();
      if (!isect.bsdf) {
        ray = isect.SpawnRay(ray.d);
        depth--;
        continue;
      }
      const Distribution1D *distrib = nullptr;
      // Sample from lights
      if (isect.bsdf->NumComponents(BxDF::Type(BxDF::Type::kAllButSpecular)) > 0) {
        Spectrum Ld = beta * UniformSampleOneLight(isect, scene, sampler, distrib);
        L += Ld;
      }
      // Sample BSDF
      Vector3 wo = -ray.d, wi;
      Float pdf;
      BSDFSample bsdf_sample;
      isect.bsdf->Sample(sampler.Get2D(), isect.wo, bsdf_sample);
      Spectrum f = bsdf_sample.f;
      pdf = bsdf_sample.pdf;
      wi = bsdf_sample.wi;
      if (IsBlack(f) || pdf == 0.0f) break;
      beta *= f * AbsDot(wi, isect.shading_frame.n) / pdf;
      specular = (bsdf_sample.sampled_type & BxDF::Type::kSpecular) != 0;
      if ((bsdf_sample.sampled_type & BxDF::Type::kSpecular) && (bsdf_sample.sampled_type & BxDF::Type::kTransmission)) {
        Float eta = isect.bsdf->eta;
        eta_scale *= (Dot(wo, isect.geo_frame.n) > 0) ? (eta * eta) : 1 / (eta * eta);
      }
      ray = isect.SpawnRay(wi);
      // Russian roulette
      Spectrum rr = beta * eta_scale;
      if (beta.MaxComp() < threshold && depth > 3) {
        Float q = std::max((Float)0.05, 1 - rr.MaxComp());
        if (sampler.Get1D() < q) break;
        beta /= 1 - q;
      }
    }
    return L;
  }
};
MIN_IMPLEMENTATION(Renderer, PathTracer, "pt")

}

