#include <min/visual/material.h>
#include <min/visual/texture.h>
#include <min/visual/intersection.h>
#include "microfacet.h"
#include "fresnel.h"

namespace min {

inline bool Refract(const Vector3 &wi, const Normal3 &n, Float eta,
                    Vector3f *wt) {
  // Compute $\cos \theta_\roman{t}$ using Snell's law
  Float cosThetaI = Dot(n, wi);
  Float sin2ThetaI = std::max(Float(0), Float(1 - cosThetaI * cosThetaI));
  Float sin2ThetaT = eta * eta * sin2ThetaI;

  // Handle total internal reflection for transmission
  if (sin2ThetaT >= 1) return false;
  Float cosThetaT = std::sqrt(1 - sin2ThetaT);
  *wt = eta * -wi + (eta * cosThetaI - cosThetaT) * Vector3f(n);
  return true;
}

class SpecularBRDF : public BxDF {
  Spectrum R;
  Float etaI, etaT;
 public:
  SpecularBRDF(Spectrum R, Float etaI, Float etaT) : R(R), etaI(etaI), etaT(etaT), BxDF(Type(kSpecular | kReflection)) {}
  Spectrum Evaluate(const Vector3f &wo, const Vector3f &wi) const override {
    return Spectrum(0);
  }
  void Sample(const Point2f &u, const Vector3f &wo, BSDFSample &sample) const override {
    sample.wi = Vector3f(-wo.x, -wo.y, wo.z);
    sample.pdf = 1;
    sample.f = FrDielectric(Frame::CosTheta(sample.wi), etaI, etaT) * R / std::abs(sample.wi.z);
  }
  Float Pdf(const Vector3 &wo, const Vector3 &wi) const override {
    return 0;
  }
};

class SpecularBTDF : public BxDF {
 public:
  SpecularBTDF(const Spectrum &T, Float etaA, Float etaB)
      : BxDF(BxDF::Type(kTransmission | kSpecular)),
        T(T),
        etaA(etaA),
        etaB(etaB),
        fresnel(etaA, etaB){}
  Spectrum Evaluate(const Vector3f &wo, const Vector3f &wi) const override {
    return Spectrum(0.f);
  }
  void Sample(const Point2f &u, const Vector3f &wo, BSDFSample &sample) const override {
    bool entering = Frame::CosTheta(wo) > 0;
    Float etaI = entering ? etaA : etaB;
    Float etaT = entering ? etaB : etaA;
    if (!Refract(wo, Faceforward(Normal3f(0, 0, 1), wo), etaI / etaT, &sample.wi)) {
      sample.f = Spectrum(0);
      return;
    }
    sample.pdf = 1;
    Spectrum ft = T * (Spectrum(1.) - fresnel.Evaluate(Frame::CosTheta(sample.wi)));
    ft *= (etaI * etaI) / (etaT * etaT);
    sample.f = ft / std::abs(Frame::CosTheta(sample.wi));
  }
  Float Pdf(const Vector3f &wo, const Vector3f &wi) const override { return 0; }

 private:
  // SpecularTransmission Private Data
  const Spectrum T;
  const Float etaA, etaB;
  const FresnelDielectric fresnel;
};

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
    Float eta = this->eta->Evaluate(si.sp).x;
    Float roux = roughness_x->Evaluate(si.sp).x;
    Float rouy = roughness_y->Evaluate(si.sp).y;
    Spectrum R = kr->Evaluate(si.sp);
    Spectrum T = kt->Evaluate(si.sp);
    si.bsdf = std::make_unique<BSDF>(si.geo_frame, si.shading_frame, eta);
    if (IsBlack(R) && IsBlack(T)) return;
    bool is_specular = roux == 0.f && rouy == 0.f;
    MicrofacetDistribution distribution(MicrofacetDistribution::Type::kGGX, roux, rouy);
    if (!IsBlack(R)) {
      if (!is_specular) {
        //si.bsdf->Add(std::make_shared<GGXBRDF>(R, distribution, eta));
      } else {
        si.bsdf->Add(std::make_shared<SpecularBRDF>(R, 1, eta));
      }
    }
    if (!IsBlack(T)) {
      if (!is_specular) {

      } else {
        si.bsdf->Add(std::make_shared<SpecularBTDF>(T, 1.f, eta));
      }
    }
  }
};
MIN_IMPLEMENTATION(Material, GlassMaterial, "glass")

}

