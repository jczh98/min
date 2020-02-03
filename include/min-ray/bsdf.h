#pragma once

#include <min-ray/interface.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>

namespace min::ray {

struct BSDFSample {

  Vector3f wi;

  Vector3f wo;

  // Relative refractive index in the sampled direction
  float eta;

  // UV associated with the point
  Point2f uv;
  // Point associated with the point
  Point3f p;

  // Measure associated with the sample
  EMeasure measure;

  BSDFSample(const Vector3f &wi)
      : wi(wi), measure(EUnknownMeasure) {}

  BSDFSample(const Vector3f &wi,
             const Vector3f &wo, EMeasure measure)
      : wi(wi), wo(wo), measure(measure) {}
};

class BSDF : public Unit {
 public:

  virtual Color3f Sample(BSDFSample &bRec, const Point2f &sample) const = 0;

  virtual Color3f Evaluate(const BSDFSample &bRec) const = 0;

  virtual float Pdf(const BSDFSample &bRec) const = 0;

  virtual bool IsDiffuse() const { return false; }
};
MIN_INTERFACE(BSDF)

}  // namespace min::ray
