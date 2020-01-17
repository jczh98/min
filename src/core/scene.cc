#include <min-ray/scene.h>
#include "lights/diffuse_area.h"

namespace min::ray {
void Scene::Preprocess() {
  auto set_light = [=](MeshTriangle *tri) {

    auto mat = tri->GetMaterial();

    if (mat && mat->emission && mat->emissionStrength) {

      auto light = std::make_shared<DiffuseAreaLight>();
      tri->light = light.get();
      
      light->SetTriangle(tri);
      lights.emplace_back(light);
      
    }
  };
  for (auto &i : meshes) {
    i->Preprocess();

    i->Foreach(set_light);
  }

  accelerator->Build(*this);
}
bool Scene::Intersect(const Ray &ray, Intersection &isect) {
  ray_counter_++;
  auto hit = accelerator->Intersect(ray, isect);
  if (hit) {
    isect.uv = isect.mesh->NormalAt(isect.uv);
    isect.material = isect.mesh->GetMaterial();
    isect.wo = -1.0f * ray.d;
    isect.ComputeLocalFrame();
    isect.p = ray.o + ray.d * isect.distance;
  }
  return hit;
}
}  // namespace min::ray