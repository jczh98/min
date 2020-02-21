#include "intersection.h"
#include <min/visual/shape.h>

namespace min {

void SurfaceIntersection::ComputeScatteringEvents() {
  shape->material->ComputeScatteringEvents(*this);
}

}

