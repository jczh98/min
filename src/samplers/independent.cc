
#include <min-ray/block.h>
#include <min-ray/sampler.h>
#include <min-ray/rng.h>

namespace min::ray {

/**
 * Independent sampling - returns independent uniformly distributed
 * random numbers on <tt>[0, 1)x[0, 1)</tt>.
 *
 * This class is essentially just a wrapper around the pcg32 pseudorandom
 * number generator. For more details on what sample generators do in
 * general, refer to the \ref Sampler class.
 */
class Independent : public Sampler {
 public:
//  Independent(const PropertyList &propList) {
//    sample_count = (size_t)propList.getInteger("sampleCount", 1);
//  }
  void initialize(const json &json) override {
    sample_count = json.at("sampleCount").get<int>();
  }

  virtual ~Independent() {}

  std::unique_ptr<Sampler> Clone() const {
    std::unique_ptr<Independent> cloned(new Independent());
    cloned->sample_count = sample_count;
    cloned->random = random;
    return std::move(cloned);
  }

  void Prepare(const ImageBlock &block) {
    random.Seed(
        block.GetOffset().x(),
        block.GetOffset().y());
  }

  void Generate() { /* No-op for this sampler */
  }
  void Advance() { /* No-op for this sampler */
  }

  float Next1D() {
    return random.UniformFloat();
  }

  Point2f Next2D() {
    return Point2f(
        random.UniformFloat(),
        random.UniformFloat());
  }

//  std::string ToString() const {
//    return tfm::format("Independent[sampleCount=%i]", sample_count);
//  }

 private:
  Rng random;
};
MIN_IMPLEMENTATION(Sampler, Independent, "independent")
//NORI_REGISTER_CLASS(Independent, "independent");
}  // namespace min::ray
