
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
    if (Fresnel(Frame::CosTheta(bsdf_sample.wi), ext_ior, int_ior)) {
      bsdf_sample.wo = Vector3f(-bsdf_sample.wi.x(), -bsdf_sample.wi.y(), bsdf_sample.wi.z());
      bsdf_sample.eta = 1.0f;
    } else {
      bsdf_sample.eta = ext_ior / int_ior;
      Vector3f n = Vector3f(0, 0, 1);
      if (Frame::CosTheta(bsdf_sample.wi) <= 0.0f) {
        bsdf_sample.eta = 1.0 / bsdf_sample.eta;
        n *= -1;
      }
      bsdf_sample.wo = (-bsdf_sample.eta * (bsdf_sample.wi - (bsdf_sample.wi.dot(n) * n)) - n * sqrt(1 - pow(bsdf_sample.eta, 2) * (1 - pow(bsdf_sample.wi.dot(n), 2)))).normalized();
      bsdf_sample.eta = int_ior / ext_ior;
    }
    bsdf_sample.measure = EDiscrete;
    return Color3f(1.0f);
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
