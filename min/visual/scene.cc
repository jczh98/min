#include "scene.h"

namespace min {

void Scene::initialize(const Json &json) {
  Unit::initialize(json);
}

bool Scene::Intersect(const Ray &ray, SurfaceIntersection &isect) const {
  if (accelerator->Intersect(ray, isect)) {
    return true;
  }
  return false;
}

bool Scene::IntersectP(const Ray &ray) const {
  return accelerator->IntersectP(ray);
}

void Scene::Build() {
  accelerator->Build();
  MIN_INFO("Current have {} primitives and {} lights.", shapes.size(), lights.size());
}
MIN_IMPLEMENTATION(Scene, Scene, "scene")

}

