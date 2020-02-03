#pragma once

#include <min-ray/interface.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>

namespace min::ray {

class Renderer : public Unit {
 public:
  // Main render process
  virtual void Render() = 0;

  std::string filename;
  std::shared_ptr<Scene> scene = nullptr;
  std::shared_ptr<Integrator> integrator = nullptr;
  int spp = 1;
};
MIN_INTERFACE(Renderer)

}  // namespace min::ray
