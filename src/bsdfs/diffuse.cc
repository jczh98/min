#include <min-ray/bsdf.h>
#include <min-ray/frame.h>
#include <min-ray/warp.h>
#include <min-ray/json.h>

namespace min::ray {

class Diffuse : public BSDF {
 public:

  void initialize(const json &json) override {
    m_albedo = json.at("color").get<Color3f>();
  }

  Color3f Evaluate(const BSDFQueryRecord &bRec) const {
    // This is a smooth BRDF -- return zero if the measure is wrong, or when queried for illumination on the backside
    if (bRec.measure != ESolidAngle || Frame::CosTheta(bRec.wi) <= 0 || Frame::CosTheta(bRec.wo) <= 0)
      return Color3f(0.0f);
    return m_albedo * INV_PI;
  }

  // Compute the density of \ref sample() wrt. solid angles
  float Pdf(const BSDFQueryRecord &bRec) const {
    if (bRec.measure != ESolidAngle || Frame::CosTheta(bRec.wi) <= 0 || Frame::CosTheta(bRec.wo) <= 0)
      return 0.0f;

    return INV_PI * Frame::CosTheta(bRec.wo);
  }

  Color3f Sample(BSDFQueryRecord &bRec, const Point2f &sample) const {
    if (Frame::CosTheta(bRec.wi) <= 0)
      return Color3f(0.0f);
    bRec.measure = ESolidAngle;

    bRec.wo = Warp::SquareToCosineHemisphere(sample);

    bRec.eta = 1.0f;

    return m_albedo;
  }

  bool isDiffuse() const {
    return true;
  }

  std::string ToString() const {
    return fmt::format(
        "Diffuse[\n"
        "  albedo = {}\n"
        "]",
        m_albedo.ToString());
  }


 private:
  Color3f m_albedo;
};
MIN_IMPLEMENTATION(BSDF, Diffuse, "diffuse")

}