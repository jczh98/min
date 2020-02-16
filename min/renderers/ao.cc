#include "sample.h"
#include <min/visual/sampling.h>

namespace min {

class AOIntegrator : public SampleRenderer {
 public:
  virtual Spectrum Li(const Ray &ray, const std::shared_ptr<Scene> &scene, Sampler &sampler) {
    SurfaceIntersection isect;
    if (!scene->Intersect(ray, isect))
      return Spectrum(0.0f);
    return Spectrum(Abs(isect.shading_frame.n));
    Normal3f n = isect.shading_frame.n;
    Vector3f x = isect.p;
    Spectrum Lt = Spectrum(0.0f,0.0f,0.0f);
    uint32_t tms = 1;
    for(uint32_t k=0;k<tms;k++){
      Vector3f dx = CosineSampleHemisphere(sampler.Get2D());
      Vector3f d = isect.ToWorld(dx);
      float tg = 1.0/tms;
      SurfaceIntersection isect_;
      if(scene->Intersect(isect.SpawnRay(d), isect_))tg=0;
      Lt += Spectrum(0.95f,0.95f,0.95f)*tg;
    }
    return Lt;
//    if (scene->Intersect(ray, isect)) {
//      auto wo = isect.ToLocal(isect.wo);
//      auto wi = CosineSampleHemisphere(sampler.Get2D());
//      if (wo.z * wi.z < 0) wi = -wi;
//      wi = isect.ToWorld(wi);
//      auto ray_ = isect.SpawnRay(wi);
//      SurfaceIntersection isect_;
//      if (!scene->Intersect(ray_, isect_) || isect.time >= occlude_distance) {
//        return Spectrum(1);
//      } else {
//        return Spectrum(0);
//      }
//    }
//    return Spectrum(0);
  }
};
MIN_IMPLEMENTATION(Renderer, AOIntegrator, "ao")

}

