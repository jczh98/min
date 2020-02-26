#include <min/visual/light.h>
#include <min/visual/image.h>
#include <min/visual/mipmap.h>
#include <min/visual/distribution.h>
#include <min/visual/scene.h>

namespace min {

class InfiniteLight : public Light {
  std::unique_ptr<MIPMap> Lmap;
  std::unique_ptr<Distribution2D> distribution;
  Point3 world_center;
  Float world_radius;
 public:
  void initialize(const Json &json) override {
    flags = LightFlags::kInfinite;
    auto filename = Value<std::string>(json, "filename", "");
    auto L = Value(json, "radiance", Vector3(1));
    auto transform = Value(json, "transform", Transform());
    light2world = transform;
    world2light = Inverse(transform);
    Point2i resolution;
    std::unique_ptr<Spectrum[]> texels(nullptr);
    if (filename != "") {
      auto texmap = GetFileResolver()->Resolve(filename);
      texels = ReadImage(texmap.string(), resolution.x, resolution.y);
      if (texels) {
        for (int i = 0; i < resolution.x * resolution.y; i++) {
          texels[i] *= L;
        }
      }
    }
    if (!texels) {
      resolution = Point2i(1);
      texels.reset(new Vector3[1]);
      texels[0] = L;
    }
    Lmap.reset(new MIPMap(resolution, texels.get()));
    int width = 2 * Lmap->Width(), height = 2 * Lmap->Height();
    std::unique_ptr<Float[]> img(new Float[width * height]);
    float fwidth = 0.5f / std::min(width, height);
    for (int64_t v = 0; v < height; v++) {
      Float vp = (v + 0.5f) / Float(height);
      Float sintheta = std::sin(kPi * (v + 0.5f) / height);
      for (int u = 0; u < width; u++) {
        Float up = (u + 0.5f) / Float(width);
        img[u + v * width] = Lmap->Lookup(Point2f(up, vp), fwidth).y;
        img[u + v * width] *= sintheta;
      }
    }
    distribution.reset(new Distribution2D(img.get(), width, height));
  }
  void Preprocess(const Scene &scene) override {
    scene.PreprocessWorldSphere(world_center, world_radius);
  }

  Spectrum Le(const Ray &ray) const override {
    Vector3 w = Normalize(world2light.ToVector(ray.d));
    Float phi = std::atan2(w.y, w.x) < 0 ? std::atan2(w.y, w.x) + 2 * kPi : std::atan2(w.y, w.x);
    Float theta = std::acos(Clamp(w.z, -1.f, 1.f));
    Point2f st(phi * kInv2Pi, theta * kInvPi);
    return Lmap->Lookup(st);
  }
  void SampleLi(const Point2f &u,
                const Intersection &isect,
                LightSample &sample,
                VisibilityTester &tester) const override {
    Float map_pdf;
    Point2f uv = distribution->SampleContinuous(u, &map_pdf);
    if (map_pdf == 0.f) {
      sample.li = Vector3(0);
      return;
    }
    Float theta = uv[1] * kPi, phi = uv[0] * 2 * kPi;
    Float costheta = std::cos(theta), sintheta = std::sin(theta);
    Float sinphi = std::sin(phi), cosphi = std::cos(phi);
    sample.wi = light2world.ToVector(Vector3(sintheta * cosphi, sintheta * sinphi, costheta));
    if (sintheta == 0.f) sample.pdf = 0;
    else sample.pdf = map_pdf / (2 * kPi * kPi * sintheta);

    tester = VisibilityTester(isect, isect.p + sample.wi * (2 * world_radius));
    sample.li = Lmap->Lookup(uv);

  }
  Float PdfLi(const Intersection &isect, const Vector3 &wiw) const override {
    Vector3 wi = world2light.ToVector(wiw);
    Float theta = std::acos(Clamp(wi.z, -1.f, 1.f));
    Float phi = std::atan2(wi.y, wi.x) < 0 ? std::atan2(wi.y, wi.x) + 2 * kPi : std::atan2(wi.y, wi.x);
    Float sintheta = std::sin(theta);
    if (sintheta == 0.f) return 0;
    return distribution->Pdf(Point2f(phi * kInv2Pi, theta * kInvPi) / (2 * kPi * kPi * sintheta));
  }
  void SampleLe(const Point2f &u1, const Point2f &u2, LightRaySample &sample) const override {

  }
  Float PdfLe(const Ray &ray, const Normal3f &n_light) const override {
    return 0;
  }
};
MIN_IMPLEMENTATION(Light, InfiniteLight, "infinite")
}