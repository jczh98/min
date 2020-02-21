#include <min/visual/material.h>
#include <min/visual/texture.h>
#include <min/visual/intersection.h>

namespace min {

class DiffuseBRDF : public BxDF {
 public:
  const Spectrum R;
  DiffuseBRDF(const Spectrum &R) : BxDF(Type(kReflection | kDiffuse)), R(R) {}
  Spectrum Evaluate(const Vector3f &wo, const Vector3f &wi) const override {
    return R * kInvPi;
  }
};
class DiffuseMaterial : public Material {
  std::shared_ptr<Texture> kd;
 public:
  void initialize(const Json &json) override {
    if (json.contains("spectrum")) {
      kd = CreateInstance<Texture>(json["spectrum"]["type"], GetProps(json["spectrum"]));
    } else {
      kd = CreateInstance<Texture>("constant", {});
    }
  }
  void ComputeScatteringEvents(SurfaceIntersection &si) const override {
    si.bsdf = std::make_unique<BSDF>(si.geo_frame, si.shading_frame);
    Spectrum albedo = kd->Evaluate(si.sp);
    si.bsdf->Add(std::make_shared<DiffuseBRDF>(albedo));
  }
};
MIN_IMPLEMENTATION(Material, DiffuseMaterial, "diffuse")

}

