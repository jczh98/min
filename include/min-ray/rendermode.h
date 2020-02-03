#pragma once

#include <min-ray/interface.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>

namespace min::ray {

class RenderMode : public Unit {
 public:
  // Release all memory
  virtual ~RenderMode() {}

  // Main render process
  virtual void Render(Scene *scene, const std::string &filename) = 0;
};
MIN_INTERFACE(RenderMode)

}  // namespace min::ray
