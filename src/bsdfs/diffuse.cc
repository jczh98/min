#include <min-ray/bsdf.h>
#include <min-ray/frame.h>
#include <min-ray/warp.h>
#include <min-ray/json.h>

namespace min::ray {

class Diffuse : public BSDF {
 public:

  void initialize(const Json &json) override {
    albedo = rjson::GetOrDefault(json, "color", Color3f(0.5f));
  }

  Color3f Evaluate(const Vector3f &wo, const Vector3f &wi) const override {
    if (Frame::CosTheta(wi) * Frame::CosTheta(wo) > 0) {
      return albedo * kPi;
    }
    return Color3f(0.0f);
  }

  // Compute the density of \ref sample() wrt. solid angles
  float Pdf(const Vector3f &wo, const Vector3f &wi) const override {
    if (Frame::CosTheta(wi) * Frame::CosTheta(wo) > 0) {
      return std::abs(Frame::CosTheta(wi)) * kInvPi;
    }
    return 0;
  }

  Color3f Sample(BSDFSample &bsdf_sample, const Point2f &sample) const override {
    bsdf_sample.wi = warp::SquareToCosineHemisphere(sample);
    bsdf_sample.sampled_type = GetBSDFType();
    if (bsdf_sample.wo.z() < 0) bsdf_sample.wo.z() *= -1;
    bsdf_sample.pdf = Pdf(bsdf_sample.wo, bsdf_sample.wi);
    return Evaluate(bsdf_sample.wo, bsdf_sample.wi);
  }

  Type GetBSDFType() const override {
    return Type(kDiffuse | kReflection);
  }

  std::string ToString() const {
    return fmt::format(
        "Diffuse[\n"
        "  albedo = {}\n"
        "]",
        albedo.ToString());
  }


 private:
  Color3f albedo;
};
MIN_IMPLEMENTATION(BSDF, Diffuse, "diffuse")

}