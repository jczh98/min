
#include <min-ray/bsdf.h>
#include <min-ray/frame.h>
#include <min-ray/warp.h>
#include <min-ray/json.h>

namespace min::ray {

class Microfacet : public BSDF {
 public:

  void initialize(const json &json) override {
    alpha = 0.1f;
    int_ior = 1.5046f;
    ext_ior = 1.000277f;
    kd = Color3f(0.5f);
    if (json.contains("alpha")) alpha = json["alpha"].get<float>();
    if (json.contains("intIOR")) int_ior = json["intIOR"].get<float>();
    if (json.contains("extIOR")) ext_ior = json["extIOR"].get<float>();
    if (json.contains("kd")) kd = json["kd"].get<Color3f>();
    ks = 1 - kd.maxCoeff();
  }

  // Beckmann-Spizzichino distribution
  float Beckmann(const Normal3f &n) const {
    float s = Frame::TanTheta(n) / alpha;
    float d = Frame::CosTheta(n);
    float d2 = d * d;
    return std::exp(-s * s) / (M_PI * alpha * alpha * d2 * d2);
  }

  float SmithG1(const Vector3f &v, const Normal3f &n) const {
    float tan_theta = Frame::TanTheta(v);

    if (tan_theta == 0.0f) return 1.0f;
    if (n.dot(v) * Frame::CosTheta(v) <= 0) return 0.0f;

    float a = 1.0f / (alpha * tan_theta);
    if (a >= 1.6f) return 1.0f;
    float a2 = a * a;
    return (3.535f * a + 2.181f * a2)
        / (1.0f + 2.276f * a + 2.577f * a2);
  }

  Color3f Evaluate(const BSDFSample &bsdf_sample) const override  {
    Vector3f wh = (bsdf_sample.wi + bsdf_sample.wo).normalized();
    float D = Beckmann(wh);
    float F = Fresnel((wh.dot(bsdf_sample.wo)), ext_ior, int_ior);
    float G = (SmithG1(bsdf_sample.wo, wh) * SmithG1(bsdf_sample.wi, wh));

    return kd * INV_PI + ks * (D * F * G)
        / (4.f * Frame::CosTheta(bsdf_sample.wo) * Frame::CosTheta(bsdf_sample.wi));
  }

  float Pdf(const BSDFSample &bsdf_sample) const override {
    Vector3f wh = (bsdf_sample.wi + bsdf_sample.wo).normalized();
    if (Frame::CosTheta(bsdf_sample.wo) <= 0)
      return 0.0f;

    float D = Beckmann(wh);
    float Jh = 1.f / (4.f * abs(wh.dot(bsdf_sample.wo)));

    return ks * D * Frame::CosTheta(wh) * Jh + (1- ks) * Frame::CosTheta(bsdf_sample.wo) * INV_PI;
  }

  Color3f Sample(BSDFSample &bsdf_sample, const Point2f &sample) const override {
    if (Frame::CosTheta(bsdf_sample.wi) <= 0)
      return Color3f(0.0f);

    if (sample.x() < ks) {
      Point2f sample(sample.x() / ks, sample.y());
      Vector3f wh = warp::SquareToBeckmann(sample, alpha);
      bsdf_sample.wo = ((2.f * wh.dot(bsdf_sample.wi) * wh) - bsdf_sample.wi).normalized();
    } else {
      Point2f sample((sample.x() - ks) / (1 - ks), sample.y());
      bsdf_sample.wo = warp::SquareToCosineHemisphere(sample);
    }

    if (Frame::CosTheta(bsdf_sample.wo) <= 0)
      return Color3f(0.0f);

    return Evaluate(bsdf_sample) / Pdf(bsdf_sample) * Frame::CosTheta(bsdf_sample.wo);
  }

  bool isDiffuse() const {
    return true;
  }

  std::string ToString() const {
    return fmt::format(
        "Microfacet[\n"
        "  alpha = {},\n"
        "  intIOR = {},\n"
        "  extIOR = {},\n"
        "  kd = {},\n"
        "  ks = {}\n"
        "]",
        alpha,
        int_ior,
        ext_ior,
        kd.ToString(),
        ks);
  }

 private:
  float alpha;
  float int_ior, ext_ior;
  float ks;
  Color3f kd;
};
MIN_IMPLEMENTATION(BSDF, Microfacet, "microfacet")

}  // namespace min::ray
