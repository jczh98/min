#include "light.h"
#include "scene.h"

namespace min {

bool VisibilityTester::Unoccluded(const Scene &scene) const {
  return !scene.IntersectP(p0.SpwanRayTo(p1));
}

}

