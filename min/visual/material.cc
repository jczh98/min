#include "material.h"
#include "sampling.h"

namespace min {

void BxDF::Sample(const Point2f &u, const Vector3f &wo, BSDFSample &sample) const {
  sample.wi = CosineSampleHemisphere(u);
  if (wo.z < 0) sample.wi.z *= -1;
  sample.pdf = Pdf(wo, sample.wi);
  sample.f = Evaluate(wo, sample.wi);
}
Float BxDF::Pdf(const Vector3 &wo, const Vector3 &wi) const {
  return wo.z * wi.z > 0 ? std::abs(Frame::CosTheta(wi)) * kInvPi : 0;
}

}

