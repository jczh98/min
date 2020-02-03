
#include <min-ray/block.h>
#include <min-ray/sampler.h>
#include <min-ray/rng.h>

namespace min::ray {

class Independent : public Sampler {
 public:

  virtual ~Independent() {}

  std::unique_ptr<Sampler> Clone() const {
    std::unique_ptr<Independent> cloned(new Independent());
    cloned->random = random;
    return std::move(cloned);
  }

  void Prepare(const ImageBlock &block) {
    random.Seed(
        block.GetOffset().x(),
        block.GetOffset().y());
  }

  void Generate() {}
  void Advance() {}

  float Next1D() {
    return random.UniformFloat();
  }

  Point2f Next2D() {
    return Point2f(
        random.UniformFloat(),
        random.UniformFloat());
  }

 private:
  Rng random;
};
MIN_IMPLEMENTATION(Sampler, Independent, "independent")

}  // namespace min::ray
