#pragma once

#include "defs.h"

namespace min {

class Filter : public Unit {
 public:
  Vector2 radius, inv_radisu;
  virtual Float Evaluate(const Point2 &p) const = 0;
};
MIN_INTERFACE(Filter)

}


