#include <min-ray/integrator.h>
#include <min-ray/scene.h>
#include <min-ray/light.h>
#include <min-ray/bsdf.h>
#include <min-ray/sampler.h>

namespace min::ray {

class PathIntegrator : public Integrator {
 public:
  Color3f Li(const std::shared_ptr<Scene> scene,
             Sampler *sampler, const Ray3f &ray) const {
    // Initial radiance and throughput
    Color3f li = 0, t = 1;
    Ray3f new_ray = ray;
    float prob = 1, w_mats = 1, w_nee = 1;
    Color3f f(1, 1, 1);

    while (true) {
      Intersection its;

      // TODO environment light
      if (!scene->Intersect(new_ray, its)) {
        return li;
      }

      // Emitted
      Color3f le = 0;
      if (its.mesh->IsLight()) {
        LightRaySample light_sample(new_ray.o, its.p, its.shading_frame.n);
        le = its.mesh->GetLight()->Evaluate(light_sample);
      }
      li += t * w_mats * le;
      // Russian roulette
      prob = std::min(t.maxCoeff(), .99f);
      if (sampler->Next1D() >= prob)
        return li;

      t /= prob;

      // Next event estimation
      Color3f l_nee = 0;
      auto const & n = scene->lights.size();
      size_t index = std::min(static_cast<size_t>(std::floor(n*sampler->Next1D())),n-1);
      const auto random_light = scene->lights[index];

      // Reflected
      LightRaySample light_sample_nee;
      light_sample_nee.ref = its.p;
      Color3f li_nee = random_light->SampleLe(light_sample_nee, sampler->Next2D()) * scene->lights.size();
      float pdf_nee = random_light->Pdf(light_sample_nee);

      // BSDF
      BSDFSample bsdf_sample_nee(its.shading_frame.ToLocal(-new_ray.d), its.shading_frame.ToLocal(light_sample_nee.wi), ESolidAngle);
      bsdf_sample_nee.uv = its.uv;
      Color3f f_nee = its.mesh->GetBSDF()->Evaluate(bsdf_sample_nee);
      if (pdf_nee + its.mesh->GetBSDF()->Pdf(bsdf_sample_nee) != 0)
        w_nee = pdf_nee / (pdf_nee + its.mesh->GetBSDF()->Pdf(bsdf_sample_nee));

      // Check if shadow ray is occluded
      Intersection its_nee;
      if (!scene->Intersect(light_sample_nee.shadowRay, its_nee)) {
        l_nee = f_nee * li_nee * std::max(0.f, Frame::CosTheta(its.shading_frame.ToLocal(light_sample_nee.wi)));
      }

      li += t * w_nee * l_nee;

      // Bsdf sampling
      BSDFSample bsdf_sample(its.shading_frame.ToLocal(-new_ray.d));
      Color3f f = its.mesh->GetBSDF()->Sample(bsdf_sample, sampler->Next2D());
      t *= f;
      // Shoot next ray
      new_ray = Ray3f(its.p, its.ToWorld(bsdf_sample.wo));

      // Mats weights for next Le
      float pdf_mats = its.mesh->GetBSDF()->Pdf(bsdf_sample);

      Intersection new_its;
      if (scene->Intersect(new_ray, new_its)) {
        if (new_its.mesh->IsLight()) {
          LightRaySample light_sample_mats = LightRaySample(its.p, new_its.p, new_its.shading_frame.n);
          if (pdf_mats + new_its.mesh->GetLight()->Pdf(light_sample_mats) != 0) {
            w_mats = pdf_mats / (pdf_mats + new_its.mesh->GetLight()->Pdf(light_sample_mats));
          }
        }
      }

      if (bsdf_sample.measure == EDiscrete) {
        w_mats = 1;
        w_nee = 0;
      }
    }

  }

  std::string ToString() const {
    return "PathIntegrator[]";
  }

};
MIN_IMPLEMENTATION(Integrator, PathIntegrator, "path")

}

