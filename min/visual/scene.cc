#include "scene.h"

namespace min {

void Scene::initialize(const Json &json) {
  Unit::initialize(json);
}

bool Scene::Intersect(const Ray &ray, SurfaceIntersection &isect) const {
  return accelerator->Intersect(ray, isect);
}

bool Scene::IntersectP(const Ray &ray) const {
  return accelerator->IntersectP(ray);
}

void Scene::Build() {
  accelerator->Build();
}
MIN_IMPLEMENTATION(Scene, Scene, "scene")

}

