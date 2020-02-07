#pragma once

#include "common.h"
#include "geometry.h"
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace min::ray {

class AtomicFloat {
 public:
  // AtomicFloat Public Methods
  explicit AtomicFloat(Float v = 0) { bits = FloatToBits(v); }
  operator Float() const { return BitsToFloat(bits); }
  Float operator=(Float v) {
    bits = FloatToBits(v);
    return v;
  }
  void Add(Float v) {
#ifdef MIN_USE_DOUBLE
    uint64_t oldBits = bits, newBits;
#else
    uint32_t oldBits = bits, newBits;
#endif
    do {
      newBits = FloatToBits(BitsToFloat(oldBits) + v);
    } while (!bits.compare_exchange_weak(oldBits, newBits));
  }

 private:
// AtomicFloat Private Data
#ifdef MIN_USE_DOUBLE
  std::atomic<uint64_t> bits;
#else
  std::atomic<uint32_t> bits;
#endif
};

}
