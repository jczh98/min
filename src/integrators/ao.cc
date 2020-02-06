#pragma once

#include <min-ray/integrator.h>
#include <min-ray/scene.h>
#include <min-ray/warp.h>

namespace min::ray {

class AmbientOcclusion : public Integrator {
 public:
  Color3f Li(const std::shared_ptr<Scene> scene,
             Sampler *sampler, const Ray3f &ray) const override {
    Intersection its;
    if (!scene->Intersect(ray, its))
      return Color3f(0.0f);

    Normal3f n = its.shading_frame.n;
    Vector3f x = its.p;
    Color3f Lt = Color3f(0.0f,0.0f,0.0f);
    uint32_t tms = 1;
    for(uint32_t k=0;k<tms;k++){
      Vector3f dx = warp::SquareToCosineHemisphere(sampler->Next2D());
      Vector3f d = its.ToWorld(dx);
      float tg = 1.0/tms;
      if(scene->Intersect(Ray3f(x,d,1e-4,20)))tg=0;
      Lt += Color3f(0.95f,0.95f,0.95f)*tg;
    }
    return Lt;
  }
};
MIN_IMPLEMENTATION(Integrator, AmbientOcclusion, "ao")

}