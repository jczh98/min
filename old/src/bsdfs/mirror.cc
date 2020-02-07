
#include <min-ray/bsdf.h>
#include <min-ray/frame.h>

namespace min::ray {

// Ideal mirror BRDF
class Mirror : public BSDF {
 public:

  Color3f Evaluate(const BSDFSample &) const override{
    return Color3f(0.0f);
  }

  float Pdf(const BSDFSample &) const override{
    return 0.0f;
  }

  Color3f Sample(BSDFSample &bsdf_sample, const Point2f &) const override {
    if (Frame::CosTheta(bsdf_sample.wi) <= 0)
      return Color3f(0.0f);

    // Reflection in local coordinates
    bsdf_sample.wo = Vector3f(
        -bsdf_sample.wi.x(),
        -bsdf_sample.wi.y(),
        bsdf_sample.wi.z());
    bsdf_sample.measure = EDiscrete;

    // Relative index of refraction
    bsdf_sample.eta = 1.0f;

    return Color3f(1.0f);
  }

  std::string ToString() const {
    return "Mirror[]";
  }
};
MIN_IMPLEMENTATION(BSDF, Mirror, "mirror")

}  // namespace min::ray
