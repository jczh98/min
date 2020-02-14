#pragma once

#include "shape.h"

namespace min {

class Aggregate : public Unit {
 public:
  std::vector<std::shared_ptr<Shape>> shapes;
};
MIN_INTERFACE(Aggregate)

}


