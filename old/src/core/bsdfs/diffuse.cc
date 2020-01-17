#include "diffuse.h"
#include <min-ray/sampling.h>

namespace min::ray {

Spectrum DiffuseBSDF::Evaluate(const ShadingPoint &point, const Vector3 &wo, const Vector3 &wi) const {
  if (wo.y * wi.y > 0)
    return Spectrum(color->Evaluate(point) * InvPi);
  return {};
}

void DiffuseBSDF::Sample(Point2 u, const ShadingPoint &sp, BSDFSample &sample) const {
  sample.wi = CosineHemisphereSampling(u);
  sample.sample_type = BSDF::Type(sample.sample_type | GetBSDFType());
  if (sample.wo.y * sample.wi.y < 0) {
    sample.wi.y = -sample.wi.y;
  }
  sample.pdf = std::abs(sample.wi.y * InvPi);
  sample.f = Evaluate(sp, sample.wo, sample.wi);
  sample.sample_type = BSDF::Type(int(BSDF::Type::EReflection) | int(BSDF::Type::EDiffuse));
}

Float DiffuseBSDF::EvaluatePdf(const ShadingPoint &point, const Vector3 &wo, const Vector3 &wi) const {
  if (wo.y * wi.y > 0)
    return std::abs(wi.y * InvPi);
  return 0;
}

}