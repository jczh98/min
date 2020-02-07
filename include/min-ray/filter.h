#pragma once

#include "common.h"
#include "geometry.h"
#include "interface.h"

namespace min::ray {

class Filter : public Unit {
 public:
  Vector2f radius, inv_radisu;
  virtual Float Evaluate(const Point2f &p) const = 0;
};
MIN_INTERFACE(Filter)

}


