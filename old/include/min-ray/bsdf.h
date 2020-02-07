#pragma once

#include <min-ray/interface.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>

namespace min::ray {

struct BSDFSample;

class BSDF : public Unit {
 public:

  enum Type {
    kNone = 0,
    kReflection = 1 << 0,
    kTransmission = 1 << 1,
    kDiffuse = 1 << 2,
    kGlossy = 1 << 3,
    kSpecular = 1 << 4,
    kAll = kDiffuse | kGlossy | kSpecular | kReflection | kTransmission
  };

  virtual Color3f Sample(BSDFSample &bRec, const Point2f &sample) const = 0;

  virtual Color3f Evaluate(const Vector3f &wo, const Vector3f &wi) const = 0;

  virtual float Pdf(const Vector3f &wo, const Vector3f &wi) const = 0;

  virtual bool IsSpecular() const { return GetBSDFType() & kSpecular; }

  virtual Type GetBSDFType() const = 0;
};
MIN_INTERFACE(BSDF)

struct BSDFSample {
  Vector3f wi;
  Vector3f wo;
  float pdf;
  float eta; // Relative refractive index in the sampled direction
  Point2f uv; // UV associated with the point
  Point3f p; // Point associated with the point
  EMeasure measure; // Measure associated with the sample
  BSDF::Type sampled_type = BSDF::kNone;

  BSDFSample(const Vector3f &wo)
      : wo(wo) {}

  BSDFSample(const Vector3f &wi,
             const Vector3f &wo, EMeasure measure)
      : wi(wi), wo(wo), measure(measure) {}
};
}  // namespace min::ray
