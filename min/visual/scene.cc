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
  world_bound = accelerator->WorldBound();
  for (const auto &light : lights) {
    light->Preprocess(*this);
    if (light->flags & LightFlags::kInfinite) {
      infinite_lights.emplace_back(light);
    }
  }
  MIN_INFO("Current have {} primitives and {} lights.", shapes.size(), lights.size());
}

void Scene::PreprocessWorldSphere(Vector3 &center, Float &radius) const {
  center = (world_bound.pmax + world_bound.pmin) / 2.f;
  radius = Inside(center, world_bound) ? (center - world_bound.pmax).Length() : 0;
}

MIN_IMPLEMENTATION(Scene, Scene, "scene")

}

