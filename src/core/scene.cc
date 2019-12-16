#include <min-ray/scene.h>

namespace min::ray {
void Scene::Preprocess() {
  for (auto &i : primitives_) {
    if (i->GetAreaLight()) {
      lights_.emplace_back(i->GetAreaLight());
    }
  }
}
bool Scene::Intersect(const Ray &ray, Intersection &isect) {
  ray_counter_++;
  auto hit = accelerator_->Intersect(ray, isect);
  if (hit) {
    isect.p = ray.o + ray.d * isect.distance;
  }
  return hit;
}
}  // namespace min::ray