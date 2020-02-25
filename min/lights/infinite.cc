#include <min/visual/light.h>

namespace min {

class InfiniteLight : public Light {
  Spectrum L;
  Point3 world_center;
  Float world_radius;
 public:
  void initialize(const Json &json) override {
    flags = LightFlags::kInfinite;
  }
  void Preprocess(const Scene &scene) override {

  }
  Spectrum Le(const Ray &ray) const override {
    return L;
  }
  void SampleLi(const Point2f &u,
                const Intersection &isect,
                LightSample &sample,
                VisibilityTester &tester) const override {

  }
  Float PdfLi(const Intersection &isect, const Vector3 &wi) const override {
    return 0;
  }
  void SampleLe(const Point2f &u1, const Point2f &u2, LightRaySample &sample) const override {

  }
  Float PdfLe(const Ray &ray, const Normal3f &n_light) const override {
    return 0;
  }
};
MIN_IMPLEMENTATION(Light, InfiniteLight, "infinite")
}