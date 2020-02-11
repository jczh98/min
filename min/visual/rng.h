#pragma once

#include <cstdint>
#include <limits>

#define PCG32_DEFAULT_STATE  0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL
#define PCG32_MULT           0x5851f42d4c957f2dULL

namespace min {

// https://en.wikipedia.org/wiki/Permuted_congruential_generator
class Rng {
 private:
  uint64_t state;
  uint64_t inc;
  static uint64_t const multiplier = 6364136223846793005u;
  static uint64_t const increment = 1442695040888963407u;

  static uint32_t rotr32(uint32_t x, unsigned r) {
    return x >> r | x << (-r & 31);
  }

  uint32_t pcg32(void) {
    uint64_t x = state;
    unsigned count = (unsigned) (x >> 59);        // 59 = 64 - 5

    state = x * PCG32_MULT + inc;
    x ^= x >> 18;                                // 18 = (64 - 27)/2
    return rotr32((uint32_t) (x >> 27), count);    // 27 = 32 - 5
  }

 public:
  explicit Rng() : state(PCG32_DEFAULT_STATE),  inc(PCG32_DEFAULT_STREAM) {}

  Rng(uint64_t initstate, uint64_t initseq = 1u) {
    Seed(initstate, initseq);
  }

  void Seed(uint64_t initstate, uint64_t initseq = 1) {
    state = 0U;
    inc = (initseq << 1u) | 1u;
    pcg32();
    state += initstate;
    pcg32();
  }

  uint32_t UniformUInt32() {
    return pcg32();
  }

  float UniformFloat() {
    return float(UniformUInt32()) / std::numeric_limits<uint32_t>::max();
  }
};
}