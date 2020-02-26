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

  Float SampleContinuous(Float u, Float *pdf, int *off = nullptr) const {
    // Find surrounding CDF segments and _offset_
    int offset = FindInterval((int)cdf.size(),
                              [&](int index) { return cdf[index] <= u; });
    if (off) *off = offset;
    // Compute offset along CDF segment
    Float du = u - cdf[offset];
    if ((cdf[offset + 1] - cdf[offset]) > 0) {
      MIN_ASSERT(cdf[offset + 1] > cdf[offset]);
      du /= (cdf[offset + 1] - cdf[offset]);
    }
    MIN_ASSERT(!std::isnan(du));

    // Compute PDF for sampled offset
    if (pdf) *pdf = (func_int > 0) ? func[offset] / func_int : 0;

    // Return $x\in{}[0,1)$ corresponding to sample
    return (offset + du) / Count();
  }

  Float DiscretePDF(int index) const {
    MIN_ASSERT(index >= 0 && index < Count());
    return func[index] / (func_int * Count());
  }
};

class Distribution2D {
 public:
  Distribution2D(const Float *func, int nu, int nv) {
    pConditionalV.reserve(nv);
    for (int v = 0; v < nv; ++v) {
      // Compute conditional sampling distribution for $\tilde{v}$
      pConditionalV.emplace_back(new Distribution1D(&func[v * nu], nu));
    }
    // Compute marginal sampling distribution $p[\tilde{v}]$
    std::vector<Float> marginalFunc;
    marginalFunc.reserve(nv);
    for (int v = 0; v < nv; ++v)
      marginalFunc.push_back(pConditionalV[v]->func_int);
    pMarginal.reset(new Distribution1D(&marginalFunc[0], nv));
  }

  Point2f SampleContinuous(const Point2f &u, Float *pdf) const {
    Float pdfs[2];
    int v;
    Float d1 = pMarginal->SampleContinuous(u[1], &pdfs[1], &v);
    Float d0 = pConditionalV[v]->SampleContinuous(u[0], &pdfs[0]);
    *pdf = pdfs[0] * pdfs[1];
    return Point2f(d0, d1);
  }
  Float Pdf(const Point2f &p) const {
    int iu = Clamp(int(p[0] * pConditionalV[0]->Count()), 0,
                   pConditionalV[0]->Count() - 1);
    int iv =
        Clamp(int(p[1] * pMarginal->Count()), 0, pMarginal->Count() - 1);
    return pConditionalV[iv]->func[iu] / pMarginal->func_int;
  }

 private:
  std::vector<std::unique_ptr<Distribution1D>> pConditionalV;
  std::unique_ptr<Distribution1D> pMarginal;
};

}
