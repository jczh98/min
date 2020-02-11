#pragma once

#include "defs.h"

namespace min {

class Renderer : public Unit {
 protected:
  std::shared_ptr<Scene> scene = nullptr;
 public:
  virtual void Render() = 0;
  void SetScene(const std::shared_ptr<Scene> &scene) { this->scene = scene; }
};
MIN_INTERFACE(Renderer)

}

