#include <min-ray/scene.h>

namespace min::ray {
void Scene::Preprocess() {
  accelerator_ = std::make_shared<Accelerator>();
  accelerator_->Build(*this);
}
bool Scene::Intersect(const Ray &ray, Intersection &isect) {
  if (accelerator_->Intersect(ray, isect)) {
    isect.n = isect.shape->NormalAt(isect.uv);
    isect.ComputeLocalFrame();
    return true;
  }
  return false;
}
}  // namespace min::ray