
#include <min-ray/bsdf.h>
#include <min-ray/frame.h>

namespace min::ray {

/// Ideal dielectric BSDF
class Dielectric : public BSDF {
 public:
  Dielectric(const PropertyList &propList) {
    /* Interior IOR (default: BK7 borosilicate optical glass) */
    m_intIOR = propList.getFloat("intIOR", 1.5046f);

    /* Exterior IOR (default: air) */
    m_extIOR = propList.getFloat("extIOR", 1.000277f);
  }

  Color3f Evaluate(const BSDFQueryRecord &) const {
    /* Discrete BRDFs always evaluate to zero in Nori */
    return Color3f(0.0f);
  }

  float Pdf(const BSDFQueryRecord &) const {
    /* Discrete BRDFs always evaluate to zero in Nori */
    return 0.0f;
  }

  Color3f Sample(BSDFQueryRecord &bRec, const Point2f &sample) const {
    throw NoriException("Unimplemented!");
  }

  std::string ToString() const {
    return tfm::format(
        "Dielectric[\n"
        "  intIOR = %f,\n"
        "  extIOR = %f\n"
        "]",
        m_intIOR, m_extIOR);
  }

 private:
  float m_intIOR, m_extIOR;
};

NORI_REGISTER_CLASS(Dielectric, "dielectric");
}  // namespace min::ray
