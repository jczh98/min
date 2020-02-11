#include "scene.h"

namespace min {

void Scene::initialize(const Json &json) {
  Unit::initialize(json);
}

bool Scene::Intersect(const Ray &ray, SurfaceIntersection &isect) const {
  return false;
}
MIN_IMPLEMENTATION(Scene, Scene, "scene")

}

