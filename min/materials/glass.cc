#include <min/visual/material.h>
#include <min/visual/texture.h>
#include "microfacet.h"
#include "fresnel.h"

namespace min {

class GGXBRDF : public BxDF {
  Spectrum R;
  MicrofacetDistribution distrib;
  Float eta;
 public:
  GGXBRDF(Spectrum R, const MicrofacetDistribution &distribution, Float eta) : R(R), distrib(distribution), eta(eta), BxDF(Type(kReflection | kGlossy)) {}
  Spectrum Evaluate(const Vector3f &wo, const Vector3f &wi) const override {
    Float costhetaO = std::abs(Frame::CosTheta(wo)), costhetaI = std::abs(Frame::CosTheta(wi));
    Normal3 wh = wi + wo;
    if (costhetaI == 0.f || costhetaO == 0.f) return Spectrum(0);
    if (wh.x == 0.f && wh.y == 0.f && wh.z == 0.f) return Spectrum(0);
    wh = Normalize(wh);
    return R * distrib.Evaluate(wh) * distrib.SmithG(wo, wi, wh) *
        FrDielectric(Dot(wi, Faceforward(wh, Vector3(0, 0, 1))), 1, eta) /
        (4 * costhetaI * costhetaO);
  }

  void Sample(const Point2f &u, const Vector3f &wo, BSDFSample &sample) const override {
    if (wo.z == 0) {
      sample.f = Spectrum(0);
      return;
    }
    Vector3 wh = distrib.Sample(u, wo);
    if (Dot(wo, wh) < 0) {
      sample.f = Spectrum(0);
      return;
    }
    sample.wi = -wo + 2 * Dot(wo, wh) * wh;
    if (!(wo.z * sample.wi.z > 0.f)) {
      sample.f = Spectrum(0);
      return;
    }
    sample.pdf = distrib.Pdf(wo, wh) / (4 * Dot(wo, wh));
    sample.f = Evaluate(wo, sample.wi);
  }
  Float Pdf(const Vector3 &wo, const Vector3 &wi) const override {
    if (!(wo.z * wi.z > 0)) return 0;
    Vector3 wh = Normalize(wo + wi);
    return distrib.Pdf(wo, wh) / (4 * Dot(wo, wh));
  }
};

class GlassMaterial : public Material {
  std::shared_ptr<Texture> kr, kt, eta, roughness_x, roughness_y;
 public:
  void initialize(const Json &json) override {
    kr = CreateInstance<Texture>(json["kr"]["type"], GetProps(json["kr"]));
    kt = CreateInstance<Texture>(json["kt"]["type"], GetProps(json["kt"]));
    eta = CreateInstance<Texture>(json["eta"]["type"], GetProps(json["eta"]));
    roughness_x = CreateInstance<Texture>(json["roughness_x"]["type"], GetProps(json["roughness_x"]));
    roughness_y = CreateInstance<Texture>(json["roughness_y"]["type"], GetProps(json["roughness_y"]));
  }
  void ComputeScatteringEvents(SurfaceIntersection &si) const override {

  }
};
}

