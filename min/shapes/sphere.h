#pragma once

#include <min/visual/shape.h>
#include <min/visual/intersection.h>

namespace min {

class Sphere : public Shape{
  const Float radius;
  const Point3f center;
 public:
  Sphere(const Transform &ObjectToWorld, const Transform &WorldToObject,
         Float radius, const Vector3f &center)
      : Shape(ObjectToWorld, WorldToObject),
        radius(radius),
        center(center) {}
  Bounds3f WorldBound() const override {
    return object2world.ToBounds3(ObjectBound());
  }
  Bounds3f ObjectBound() const override {
    return Bounds3f(Point3f(center[0] - radius, center[1] - radius, center[2] - radius),
                    Point3f(center[0] + radius, center[1] + radius, center[2] + radius));
  }
  bool Intersect(const Ray &ray, SurfaceIntersection &isect) const override {
    auto oc = ray.o - center;
    auto a = Dot(ray.d, ray.d);
    auto b = 2 * Dot(ray.d, oc);
    auto c = Dot(oc, oc) - radius * radius;
    auto delta = b * b - 4 * a * c;
    if (delta < 0) {
      return false;
    }
    auto t1 = (-b - std::sqrt(delta)) / (2 * a);
    if (t1 >= ray.time) {
      if (t1 < ray.tmax) {
        isect.time = t1;
        auto p = ray.o + t1 * ray.d;
        isect.geo_frame = Frame(p - center);
        isect.shading_frame = isect.geo_frame;
        isect.shape = this;
        return true;
      }
    }
    auto t2 = (-b + std::sqrt(delta)) / (2 * a);
    if (t2 >= ray.time) {
      if (t2 < ray.tmax) {
        isect.time = t2;
        auto p = ray.o + t2 * ray.d;
        isect.geo_frame = Frame(p - center);
        isect.shading_frame = isect.geo_frame;
        isect.shape = this;
        return true;
      }
    }
    return false;
  }
  Float Area() const override {
    return 4 * kPi * radius * radius;
  }
  void Sample(const Point2f &u, SurfaceSample &sample) const override {

  }
};

}


