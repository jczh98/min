#include <min/visual/material.h>
#include <min/visual/texture.h>
#include <min/visual/intersection.h>

namespace min {

class MirrorBRDF : public BxDF {
  const Spectrum R;
 public:
  MirrorBRDF(const Spectrum &R) : R(R), BxDF(Type(kReflection | kSpecular)) {}
  Spectrum Evaluate(const Vector3f &wo, const Vector3f &wi) const override {
    return min::Spectrum(0);
  }
  void Sample(const Point2f &u, const Vector3f &wo, BSDFSample &sample) const override {
    sample.wi = Vector3f(-wo.x, -wo.y, wo.z);
    sample.pdf = 1;
    sample.f = R / std::abs(sample.wi.z);
  }
  Float Pdf(const Vector3 &wo, const Vector3 &wi) const override {
    return 0;
  }
};

class MirrorMaterial : public Material {
  std::shared_ptr<Texture> kr;
 public:
  void initialize(const Json &json) override {
    if (json.contains("spectrum")) {
      kr = CreateInstance<Texture>(json["spectrum"]["type"], GetProps(json["spectrum"]));
    } else {
      kr = CreateInstance<Texture>("constant", {});
    }
  }
  void ComputeScatteringEvents(SurfaceIntersection &si) const override {
    si.bsdf = std::make_unique<BSDF>(si.geo_frame, si.shading_frame);
    Spectrum R = kr->Evaluate(si.sp);
    if (!IsBlack(R)) {
      si.bsdf->Add(std::make_shared<MirrorBRDF>(R));
    }
  }
};
MIN_IMPLEMENTATION(Material, MirrorMaterial, "mirror")

}

