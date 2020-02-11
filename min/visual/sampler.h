#pragma once

#include "defs.h"

namespace min {

class Sampler : public Unit {
 public:
  virtual void StartPixel(const Point2i &p) = 0;
  virtual Float Get1D() = 0;
  virtual Point2f Get2D() = 0;
  virtual std::unique_ptr<Sampler> Clone() = 0;
  int64_t spp;
};
MIN_INTERFACE(Sampler)

}
