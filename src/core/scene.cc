#include <min-ray/scene.h>
#include "accelerators/sahbvh.h"

namespace min::ray {
void Scene::Preprocess() {
  accelerator_ = std::make_shared<BVHAccelerator>();
  accelerator_->Build(*this);
}
bool Scene::Intersect(const Ray &ray, Intersection &isect) {
  if (accelerator_->Intersect(ray, isect)) {
    isect.ns = isect.shape->NormalAt(isect.uv);
    isect.ComputeLocalFrame();
    return true;
  }
  return false;
}
}  // namespace min::ray