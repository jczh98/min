#pragma once

#include <min-ray/interface.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>

namespace min::ray {

struct BSDFQueryRecord {

  Vector3f wi;

  Vector3f wo;

  // Relative refractive index in the sampled direction
  float eta;

  // Measure associated with the sample
  EMeasure measure;

  BSDFQueryRecord(const Vector3f &wi)
      : wi(wi), measure(EUnknownMeasure) {}

  BSDFQueryRecord(const Vector3f &wi,
                  const Vector3f &wo, EMeasure measure)
      : wi(wi), wo(wo), measure(measure) {}
};

class BSDF : public Unit {
 public:

  virtual Color3f Sample(BSDFQueryRecord &bRec, const Point2f &sample) const = 0;

  virtual Color3f Evaluate(const BSDFQueryRecord &bRec) const = 0;

  virtual float Pdf(const BSDFQueryRecord &bRec) const = 0;

  virtual bool IsDiffuse() const { return false; }
};
MIN_INTERFACE(BSDF)

}  // namespace min::ray
