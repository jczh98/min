#include <min/visual/sampler.h>
#include <min/visual/rng.h>

namespace min {

class RandomSampler : public Sampler {
  Rng rng;
 public:
  void initialize(const Json &json) override {
    int seed = Value(json, "seed", 0);
    spp = Value(json, "spp", 1);
  }
  void StartPixel(const Point2i &p) override {

  }
  Float Get1D() override {
    return rng.UniformFloat();
  }
  Point2f Get2D() override {
    return Point2f(Get1D(), Get1D());
  }
  std::unique_ptr<Sampler> Clone() override {
    std::unique_ptr<RandomSampler> cloned(new RandomSampler());
    cloned->rng = rng;
    cloned->spp = spp;
    return std::move(cloned);
  }
};
MIN_IMPLEMENTATION(Sampler, RandomSampler, "random")

}

