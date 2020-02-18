#pragma once

#include "defs.h"
#include "spectrum.h"

namespace min {

struct ShadingPoint {
  Point2 texcoords;
  Normal3 ng;
  Normal3 ns;
};

class Texture : public Unit {
 public:
  virtual Spectrum Evaluate(const ShadingPoint &sp) const = 0;
};
MIN_INTERFACE(Texture)

}


