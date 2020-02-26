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
  std::vector<std::shared_ptr<Light>> lights;
  std::vector<std::shared_ptr<Light>> infinite_lights;
  std::vector<std::shared_ptr<Shape>> shapes;
  ~Scene() {
    lights.clear();
    shapes.clear();
  }
  const Bounds3f &WorldBound() { return world_bound; }
  bool Intersect(const Ray &ray, SurfaceIntersection &isect) const;
  bool IntersectP(const Ray &ray) const;
  void PreprocessWorldSphere(Vector3 &center, Float &radius) const;
  void SetCamera(const std::shared_ptr<Camera> &camera) { this->camera = camera; }
  void SetAccelerator(const std::shared_ptr<Accelerator> &accel) { accelerator = accel; }
  void AddShape(const std::vector<std::shared_ptr<Shape>> &shapes_) {
    shapes.insert(shapes.end(), shapes_.begin(), shapes_.end());
    accelerator->AddShape(shapes_);
  }
  void AddLights(const std::vector<std::shared_ptr<Light>> &lights_) {
    lights.insert(lights.end(), lights_.begin(), lights_.end());
  }
  void AddLight(const std::shared_ptr<Light> &light) {
    lights.emplace_back(light);
  }
  void Build();
  void initialize(const Json &json) override;
};
MIN_INTERFACE(Scene)

}


