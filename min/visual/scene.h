#pragma once

#include "defs.h"
#include "geometry.h"
#include "camera.h"
#include "shape.h"
#include "accel.h"

namespace min {

class Scene : public Unit{
  Bounds3f world_bound;
 public:
  std::shared_ptr<Camera> camera;
  std::shared_ptr<Accelerator> accelerator;
  const Bounds3f &WorldBound() { return world_bound; }
  bool Intersect(const Ray &ray, SurfaceIntersection &isect) const;
  void SetCamera(const std::shared_ptr<Camera> &camera) { this->camera = camera; }
  void SetAccelerator(const std::shared_ptr<Accelerator> &accel) { accelerator = accel; }
  void AddShape(const std::vector<std::shared_ptr<Shape>> &shapes) { accelerator->AddShape(shapes); }
  void Build();
  void initialize(const Json &json) override;
};
MIN_INTERFACE(Scene)

}


