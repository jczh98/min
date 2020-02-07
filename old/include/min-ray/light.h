#pragma once

#include <min-ray/interface.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>
#include <min-ray/scene.h>

namespace min::ray {

struct VisibilityTester {
  Ray3f r;

  void SetSegment(const Point3f &p1, float eps1,
                  const Point3f &p2, float eps2) {
    //calculate the direction
    Vector3f d = (p2 - p1);
    //calculate the distance
    float dist = (p2 - p1).norm();
    //create the ray segment
    r = Ray3f(p1, d / dist, eps1, dist * (1.f - eps2)); /*, time);*/
  }

  void SetRay(const Point3f &p, const Vector3f &w/* , float eps, float time*/) {
    r = Ray3f(p, w);
  }

  bool Unoccluded(const Scene *scene) const {
    return !scene->Intersect(r);
  }
};

struct LightRaySample {
  // Origin point from which we sample the emitter
  Point3f ref;
  // Sampled point on the emitter
  Point3f p;
  // Normal at the emitter point
  Normal3f n;
  // Direction between the hit point and the emitter point
  Vector3f wi;
  // Probability
  float pdf;
  // Shadow ray
  Ray3f shadowRay;

  // Create an unitialized query record
  LightRaySample() { }

  // Create a new query record that can be used to sample a emitter
  LightRaySample(const Point3f &ref) : ref(ref) { }

  LightRaySample(const Point3f &ref, const Point3f &p, const Normal3f &n) :
      ref(ref), p(p), n(n) {
    wi = (p - ref).normalized();
  }
};

class Light : public Unit {
 protected:
  Mesh *mesh = nullptr;
 public:
  virtual Color3f SampleLe(LightRaySample &lRec, const Point2f &sample) const = 0;
  virtual Color3f Evaluate(const LightRaySample &lRec) const = 0;
  virtual float Pdf(const LightRaySample &lRec) const = 0;
  void SetMesh(Mesh *mesh) { this->mesh = mesh;}
};
MIN_INTERFACE(Light)

}  // namespace min::ray
