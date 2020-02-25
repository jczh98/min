#pragma once

#include "defs.h"
#include "spectrum.h"
#include "transform.h"
#include "intersection.h"

namespace min {

enum LightFlags {
  kDeltaPosition = 1 << 0,
  kDeltaDirection = 1 << 1,
  kArea = 1 << 2,
  kInfinite = 1 << 3
};

inline bool IsDeltaLight(int flags) {
  return flags & LightFlags::kDeltaDirection || flags & LightFlags::kDeltaPosition;
}

struct LightSample {
  Vector3 wi;
  Float pdf;
  Spectrum li;
};

struct LightRaySample {
  Ray ray;
  Float time;
  Spectrum le;
  Normal3 n_light;
  Float pdf_pos;
  Float pdf_dir;
};

class Light : public Unit{
 protected:
  Transform light2world, world2light;
 public:
  int flags;
  int n_samples;
  virtual void Preprocess(const Scene &scene) {}
  virtual void SampleLi(const Point2f &u, const Intersection &isect, LightSample &sample, VisibilityTester &tester) const = 0;
  virtual Float PdfLi(const Intersection &isect, const Vector3 &wi) const = 0;
  virtual void SampleLe(const Point2f &u1, const Point2f &u2, LightRaySample &sample) const = 0;
  virtual Float PdfLe(const Ray &ray, const Normal3f &n_light) const = 0;
  virtual void SetShape(const std::shared_ptr<Shape> &shape) {}
  virtual Spectrum Le(const Ray &ray) const { return Spectrum(0); }
  virtual Spectrum L(const Intersection &isect, const Vector3 &w) const { return Spectrum(0); }
};
MIN_INTERFACE(Light)

class VisibilityTester {
 public:
  Point3 p1;
  Intersection p0;
  VisibilityTester() {}
  VisibilityTester(const Intersection &p0, const Point3 &p1) : p0(p0), p1(p1) {}
  bool Unoccluded(const std::shared_ptr<Scene> &scene) const;
};

}

