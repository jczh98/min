
#include <min-ray/bsdf.h>
#include <min-ray/frame.h>

namespace min::ray {

// Ideal dielectric BSDF
class Dielectric : public BSDF {
 public:
  void initialize(const json &json) override {
    if (json.contains("int_ior")) {
      int_ior = json.at("int_ior").get<float>();
    } else {
      int_ior = 1.5046f;
    }
    if (json.contains("ext_ior")) {
      ext_ior = json.at("ext_ior").get<float>();
    } else {
      ext_ior = 1.000277f;
    }
  }

  Color3f Evaluate(const BSDFSample &bsdf_sample) const override {
    /* Discrete BRDFs always evaluate to zero in Nori */
    return Color3f(0.0f);
  }

  float Pdf(const BSDFSample &bsdf_sample) const override {
    /* Discrete BRDFs always evaluate to zero in Nori */
    return 0.0f;
  }

  Color3f Sample(BSDFSample &bsdf_sample, const Point2f &sample) const override {
    bsdf_sample.measure = EDiscrete;
    if (Frame::CosTheta(bsdf_sample.wi) == 0.0f)
      return Color3f(0.0f);
    float eta1, eta2;
    Vector3f n;
    if (Frame::CosTheta(bsdf_sample.wi) <= 0.0f) {
      eta1 = int_ior;
      eta2 = ext_ior;
      n = Vector3f(0, 0, -1.0f);
    }
    else {
      eta1 = ext_ior;
      eta2 = int_ior;
      n = Vector3f(0, 0, 1.0f);
    }
    float fresPos = Fresnel(fabs(Frame::CosTheta(bsdf_sample.wi)), eta1, eta2);
    if (sample.x() > fresPos) {
      float eta = eta1 / eta2;
      float cost = bsdf_sample.wi.dot(n);
      cost = sqrt(1.0f - eta * eta * (1 - cost * cost));
      Vector3f wt = - cost * n;
      wt -= eta * (bsdf_sample.wi - bsdf_sample.wi.dot(n) * n);
      bsdf_sample.wo = wt;
      bsdf_sample.eta = eta;
    }
    else {
      bsdf_sample.wo = 2 * n * bsdf_sample.wi.dot(n) - bsdf_sample.wi;
      bsdf_sample.eta = 1.0f;
    }
    if (Frame::CosTheta(bsdf_sample.wo) == 0.0f) return Color3f(0.0f);
    return Color3f(bsdf_sample.eta * bsdf_sample.eta);
  }

  std::string ToString() const {
    return fmt::format(
        "Dielectric[\n"
        "  intIOR = {},\n"
        "  extIOR = {}\n"
        "]",
        int_ior, ext_ior);
  }

 private:
  float int_ior, ext_ior;
};
MIN_IMPLEMENTATION(BSDF, Dielectric, "dielectric")

}  // namespace min::ray
