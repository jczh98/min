//#pragma once
//
//#include "defs.h"
//#include "spectrum.h"
//#include "texture.h"
//
//namespace min {
//
//class BSDF : public Unit {
// public:
//  enum Type : int {
//    kNone = 0,
//    kDiffuse = 1ULL,
//    kGlossy = 1ULL << 1U,
//    kReflection = 1ULL << 2U,
//    kTransmission = 1ULL << 3U,
//    kSpecular = 1ULL << 4U,
//    kAll = kDiffuse | kGlossy | kTransmission | kSpecular | kReflection,
//    kAllButSpecular = kAll & ~kSpecular
//  };
//
//  virtual Type GetBSDFType() const = 0;
//
//  virtual Spectrum Evaluate(const ShadingPoint &sp, const Vector3 &wo, const Vector3 &wi) const = 0;
//
//  virtual Float EvaluatePdf(const ShadingPoint &sp, const Vector3 &wo, const Vector3 &wi) const = 0;
//
//  virtual void Sample(const Point2 &u, const ShadingPoint &sp, BSDFSample &sample) const = 0;
//
//  virtual bool IsSpecular() const { return GetBSDFType() & kSpecular; }
//};
//MIN_INTERFACE(BSDF)
//
//struct BSDFSample {
//  Vector3 wo;
//  Vector3 wi;
//  float pdf = 0;
//  Spectrum f;
//  BSDF::Type sampled_type = BSDF::kNone;
//};
//
//}
//
//
