
#include <min-ray/bsdf.h>
#include <min-ray/frame.h>

namespace min::ray {

/// Ideal mirror BRDF
class Mirror : public BSDF {
 public:
  Mirror(const PropertyList &) {}

  Color3f eval(const BSDFQueryRecord &) const {
    /* Discrete BRDFs always evaluate to zero in Nori */
    return Color3f(0.0f);
  }

  float pdf(const BSDFQueryRecord &) const {
    /* Discrete BRDFs always evaluate to zero in Nori */
    return 0.0f;
  }

  Color3f sample(BSDFQueryRecord &bRec, const Point2f &) const {
    if (Frame::cosTheta(bRec.wi) <= 0)
      return Color3f(0.0f);

    // Reflection in local coordinates
    bRec.wo = Vector3f(
        -bRec.wi.x(),
        -bRec.wi.y(),
        bRec.wi.z());
    bRec.measure = EDiscrete;

    /* Relative index of refraction: no change */
    bRec.eta = 1.0f;

    return Color3f(1.0f);
  }

  std::string toString() const {
    return "Mirror[]";
  }
};

NORI_REGISTER_CLASS(Mirror, "mirror");
}  // namespace min::ray
