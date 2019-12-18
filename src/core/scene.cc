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
Light *Scene::SampleLight(const std::shared_ptr<Sampler> &sampler) const {
  auto i = std::min<int>(sampler->Get1D() * lights_.size(), lights_.size() - 1);
  if (i >= 0) {
    return lights_[i];
  }
  return nullptr;
}
}  // namespace min::ray