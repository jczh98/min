#include <min/visual/bsdf.h>
#include <min/visual/frame.h>
#include <min/visual/sampling.h>

namespace min {

class DiffuseBSDF : public BSDF {
  std::shared_ptr<Texture> albedo;
 public:
  void initialize(const Json &json) override {
    if (json.contains("spectrum")) {
      albedo = CreateInstance<Texture>(json["spectrum"]["type"], GetProps(json["spectrum"]));
    } else {
      albedo = CreateInstance<Texture>("constant", {});
    }
  }
  Type GetBSDFType() const override {
    return kDiffuse;
  }
  Spectrum Evaluate(const ShadingPoint &sp, const Vector3 &wo, const Vector3 &wi) const override {
    return albedo->Evaluate(sp) * kInvPi;
  }
  Float EvaluatePdf(const ShadingPoint &sp, const Vector3 &wo, const Vector3 &wi) const override {
    return wo.z * wi.z > 0 ? std::abs(Frame::CosTheta(wi) * kInvPi) : 0;
  }
  void Sample(const Point2 &u, const ShadingPoint &sp, BSDFSample &sample) const override {
    sample.wi = CosineSampleHemisphere(u);
    if (sample.wo.z < 0) sample.wi *= -1;
    sample.pdf = EvaluatePdf(sp, sample.wo, sample.wi);
    sample.f = Evaluate(sp, sample.wo, sample.wi);
    sample.sampled_type = BSDF::Type(BSDF::Type::kReflection | BSDF::Type::kDiffuse);
  }
};
MIN_IMPLEMENTATION(BSDF, DiffuseBSDF, "diffuse")

}

