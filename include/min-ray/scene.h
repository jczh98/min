#pragma once

#include <min-ray/accel.h>
#include <min-ray/renderer.h>

namespace min::ray {


class Scene : public Unit {
 public:

  Scene() {
    accelerator = std::make_shared<Accel>();
  }

  void AddPrimitive(const std::shared_ptr<Mesh> mesh) {
    accelerator->AddMesh(mesh);
    meshes.emplace_back(mesh);
    if (mesh->IsLight()) {
      lights.emplace_back(mesh->GetLight());
    }
  }

  void Build() {
    accelerator->Build();
  }

  bool Intersect(const Ray3f &ray, Intersection &its) const {
    return accelerator->Intersect(ray, its, false);
  }

  bool Intersect(const Ray3f &ray) const {
    Intersection its;
    return accelerator->Intersect(ray, its, true);
  }

  const BoundingBox3f &GetBoundingBox() const {
    return accelerator->GetBoundingBox();
  }

  std::vector<std::shared_ptr<Mesh>> meshes;
  std::vector<std::shared_ptr<Light>> lights;
  std::shared_ptr<Integrator> integrator = nullptr;
  std::shared_ptr<Sampler> sampler = nullptr;
  std::shared_ptr<Camera> camera = nullptr;
  std::shared_ptr<Accel> accelerator = nullptr;
};
MIN_INTERFACE(Scene)

}  // namespace min::ray
