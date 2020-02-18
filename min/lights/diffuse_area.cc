#include <min/visual/light.h>
#include <min/visual/shape.h>

namespace min {

class DiffuseAreaLight : public Light {
  std::shared_ptr<Shape> shape;
  std::shared_ptr<Texture> radiance;
 public:
  void initialize(const Json &json) override {
    flags = LightFlags::kArea;
    radiance = CreateInstance<Texture>(json["radiance"]["type"], GetProps(json["radiance"]));
  }
  void SetShape(const std::shared_ptr<Shape> &shape) override {
    this->shape = shape;
  }
  void SampleLi(const Point2f &u,
                const Intersection &isect,
                LightSample &sample,
                VisibilityTester &tester) const override {
    SurfaceSample surface_sample;
    shape->Sample(u, surface_sample);
    if (surface_sample.pdf == 0 || (surface_sample.p - isect.p).LengthSquared() == 0) {
      surface_sample.pdf = 0;
      sample.pdf = 0;
      return;
    }
    sample.wi = Normalize(surface_sample.p - isect.p);
    tester = VisibilityTester(isect, surface_sample.p);
    sample.pdf = surface_sample.pdf;
    sample.li = Dot(surface_sample.ng, -sample.wi) > 0 ? radiance->Evaluate(isect.sp) : Spectrum(0);
  }
  virtual Spectrum L(const Intersection &isect, const Vector3 &w) const {
    return Dot(isect.geo_frame.n, w) > 0 ? radiance->Evaluate(isect.sp) : Spectrum(0);
  }
  Float PdfLi(const Intersection &isect, const Vector3 &wi) const override {
    return shape->Pdf(isect, wi);
  }
  void SampleLe(const Point2f &u1, const Point2f &u2, LightRaySample &sample) const override {

  }
  Float PdfLe(const Ray &ray, const Normal3f &n_light) const override {
    return 0;
  }
};
MIN_IMPLEMENTATION(Light, DiffuseAreaLight, "diffuse_area")

}

