#include <min-ray/bsdf.h>
#include <min-ray/frame.h>
#include <min-ray/sampling.h>
#include <min-ray/json.h>

namespace min::ray {

class Diffuse : public BSDF {
 public:

  void initialize(const json &json) override {
    albedo = json.at("color").get<Color3f>();
  }

  Color3f Evaluate(const BSDFSample &bsdf_sample) const {
    // This is a smooth BRDF -- return zero if the measure is wrong, or when queried for illumination on the backside
    if (bsdf_sample.measure != ESolidAngle || Frame::CosTheta(bsdf_sample.wi) <= 0 || Frame::CosTheta(bsdf_sample.wo) <= 0)
      return Color3f(0.0f);
    return albedo * INV_PI;
  }

  // Compute the density of \ref sample() wrt. solid angles
  float Pdf(const BSDFSample &bsdf_sample) const {
    if (bsdf_sample.measure != ESolidAngle || Frame::CosTheta(bsdf_sample.wi) <= 0 || Frame::CosTheta(bsdf_sample.wo) <= 0)
      return 0.0f;

    return INV_PI * Frame::CosTheta(bsdf_sample.wo);
  }

  Color3f Sample(BSDFSample &bsdf_sample, const Point2f &sample) const {
    if (Frame::CosTheta(bsdf_sample.wi) <= 0)
      return Color3f(0.0f);
    bsdf_sample.measure = ESolidAngle;

    bsdf_sample.wo = Warp::SquareToCosineHemisphere(sample);

    bsdf_sample.eta = 1.0f;

    return albedo;
  }

  bool isDiffuse() const {
    return true;
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