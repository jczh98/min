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
    float prob = 1;

    while (true) {
      Intersection its;

      if (!scene->Intersect(new_ray, its))
        return li;

      // emitted
      if (its.mesh->IsLight()) {
        LightRaySample light_sample(new_ray.o, its.p, its.shading_frame.n);
        li += t*its.mesh->GetLight()->Evaluate(light_sample);
      }
      // Russian roulette
      prob = std::min(t[0], .99f);
      if (sampler->Next1D() >= prob)
        return li;

      t /= prob;

      // BSDF
      BSDFSample bsdf_sample(its.shading_frame.ToLocal(-new_ray.d));
      Color3f f = its.mesh->GetBSDF()->Sample(bsdf_sample, sampler->Next2D());
      t *= f;

      new_ray = Ray3f(its.p, its.toWorld(bsdf_sample.wo));
    }

  }

  std::string ToString() const {
    return "PathIntegrator[]";
  }

};
MIN_IMPLEMENTATION(Integrator, PathIntegrator, "path")

}

