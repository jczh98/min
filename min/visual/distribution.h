#pragma once

#include "defs.h"

namespace min {

struct Distribution1D {
  std::vector<Float> func, cdf;
  Float func_int;

  Distribution1D(const Float *f, int n) : func(f, f + n), cdf(n + 1) {
    cdf[0] = 0;
    for (int i = 1; i < n + 1; ++i) cdf[i] = cdf[i - 1] + func[i - 1] / n;
    func_int = cdf[n];
    if (func_int == 0) {
      for (int i = 1; i < n + 1; i++) cdf[i] = Float(i) / Float(n);
    } else {
      for (int i = 1; i < n + 1; i++) cdf[i] /= func_int;
    }
  }

  int Count() const { return (int) func.size(); }

  int SampleDiscrete(Float u, Float *pdf = nullptr) const {
    int offset = FindInterval((int) cdf.size(),
        [&](int index) { return cdf[index] <= u; });
    if (pdf) *pdf = (func_int > 0) ? func[offset] / (func_int * Count()) : 0;
    return offset;
  }

  Float DiscretePDF(int index) const {
    MIN_ASSERT(index >= 0 && index < Count());
    return func[index] / (func_int * Count());
  }
};

}
