#pragma once

#include "defs.h"
#include "geometry.h"
#include "camera.h"

namespace min {

class Scene : public Unit{
  Bounds3f world_bound;
 public:
  std::shared_ptr<Camera> camera;
  const Bounds3f &WorldBound() { return world_bound; }
  bool Intersect(const Ray &ray, SurfaceIntersection &isect) const;
  void SetCamera(const std::shared_ptr<Camera> &camera) { this->camera = camera; }
  void initialize(const Json &json) override;
};
MIN_INTERFACE(Scene)

}


