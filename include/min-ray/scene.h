#pragma once

#include <min-ray/accel.h>
#include <min-ray/rendermode.h>

namespace min::ray {


class Scene : public Unit {
 public:
  const std::shared_ptr<Accel> GetAccelerator() const { return accelerator; }

  const std::vector<std::shared_ptr<Mesh>> &GetMeshes() const { return meshes; }

  const std::vector<std::shared_ptr<Emitter>> &GetLights() const { return lights; }

  bool Intersect(const Ray3f &ray, Intersection &its) const {
    return accelerator->Intersect(ray, its, false);
  }

  bool Intersect(const Ray3f &ray) const {
    Intersection its; /* Unused */
    return accelerator->Intersect(ray, its, true);
  }

  /// \brief Return an axis-aligned box that bounds the scene
  const BoundingBox3f &getBoundingBox() const {
    return accelerator->GetBoundingBox();
  }

  std::vector<std::shared_ptr<Mesh>> meshes;
  std::vector<std::shared_ptr<Emitter>> lights;
  std::shared_ptr<Integrator> integrator = nullptr;
  std::shared_ptr<Sampler> sampler = nullptr;
  std::shared_ptr<Camera> camera = nullptr;
  std::shared_ptr<Accel> accelerator = nullptr;
  std::shared_ptr<RenderMode> rendermode = nullptr;
};
MIN_INTERFACE(Scene)

}  // namespace min::ray
