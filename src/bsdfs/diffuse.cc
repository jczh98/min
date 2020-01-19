#include <min-ray/bsdf.h>
#include <min-ray/frame.h>
#include <min-ray/warp.h>

namespace min::ray {

/**
 * \brief Diffuse / Lambertian BRDF model
 */
class Diffuse : public BSDF {
 public:
  Diffuse(const PropertyList &propList) {
    m_albedo = propList.getColor("albedo", Color3f(0.5f));
  }

  /// Evaluate the BRDF model
  Color3f Evaluate(const BSDFQueryRecord &bRec) const {
    /* This is a smooth BRDF -- return zero if the measure
           is wrong, or when queried for illumination on the backside */
    if (bRec.measure != ESolidAngle || Frame::CosTheta(bRec.wi) <= 0 || Frame::CosTheta(bRec.wo) <= 0)
      return Color3f(0.0f);

    /* The BRDF is simply the albedo / pi */
    return m_albedo * INV_PI;
  }

  /// Compute the density of \ref sample() wrt. solid angles
  float Pdf(const BSDFQueryRecord &bRec) const {
    /* This is a smooth BRDF -- return zero if the measure
           is wrong, or when queried for illumination on the backside */
    if (bRec.measure != ESolidAngle || Frame::CosTheta(bRec.wi) <= 0 || Frame::CosTheta(bRec.wo) <= 0)
      return 0.0f;

    /* Importance sampling density wrt. solid angles:
           cos(theta) / pi.

           Note that the directions in 'bRec' are in local coordinates,
           so Frame::cosTheta() actually just returns the 'z' component.
        */
    return INV_PI * Frame::CosTheta(bRec.wo);
  }

  /// Draw a a sample from the BRDF model
  Color3f Sample(BSDFQueryRecord &bRec, const Point2f &sample) const {
    if (Frame::CosTheta(bRec.wi) <= 0)
      return Color3f(0.0f);

    bRec.measure = ESolidAngle;

    /* Warp a uniformly distributed sample on [0,1]^2
           to a direction on a cosine-weighted hemisphere */
    bRec.wo = Warp::SquareToCosineHemisphere(sample);

    /* Relative index of refraction: no change */
    bRec.eta = 1.0f;

    /* eval() / pdf() * cos(theta) = albedo. There
           is no need to call these functions. */
    return m_albedo;
  }

  bool isDiffuse() const {
    return true;
  }

  /// Return a human-readable summary
  std::string ToString() const {
    return tfm::format(
        "Diffuse[\n"
        "  albedo = %s\n"
        "]",
        m_albedo.ToString());
  }

  EClassType getClassType() const { return EBSDF; }

 private:
  Color3f m_albedo;
};

NORI_REGISTER_CLASS(Diffuse, "diffuse");

}