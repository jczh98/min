#include <min-ray/integrator.h>
#include <min-ray/scene.h>
#include <min-ray/emitter.h>
#include <min-ray/bsdf.h>
#include <min-ray/sampler.h>

namespace min::ray {

class PathIntegrator : public Integrator {
 public:
  PathIntegrator(const PropertyList &props) {
    /* No parameters this time */
  }

  Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
    // Initial radiance and throughput
    Color3f li = 0, t = 1;
    Ray3f rayR = ray;
    float prob = 1;

    while (true) {
      Intersection its;

      if (!scene->Intersect(rayR, its))
        return li;

      // emitted
      if (its.mesh->IsLight()) {
        LightRayQueryRecord lRecE(rayR.o, its.p, its.shading_frame.n);
        li += t*its.mesh->GetEmitter()->Evaluate(lRecE);
      }
      // Russian roulette
      prob = std::min(t[0], .99f);
      if (sampler->Next1D() >= prob)
        return li;

      t /= prob;

      // BSDF
      BSDFQueryRecord bRec(its.shading_frame.ToLocal(-rayR.d));
      Color3f f = its.mesh->GetBSDF()->Sample(bRec, sampler->Next2D());
      t *= f;

      rayR = Ray3f(its.p, its.toWorld(bRec.wo));
    }

  }

  std::string ToString() const {
    return "PathIntegrator[]";
  }
 private:

};

NORI_REGISTER_CLASS(PathIntegrator, "path");

}

