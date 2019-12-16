// MIT License
//
// Copyright (c) 2018-2019, neverfelly <neverfelly@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "sphere.h"

namespace min::ray {
void Sphere::Sample(const Point2& u, SurfaceSample& sample) const {
  Float theta = 2 * Pi * u[0];
  Float v = 2 * u[1] - 1;
  auto t = std::sqrt(1 - v * v);
  sample.p = Point3(v * std::sin(theta), v * std::cos(theta), v) * radius_ + center_;
  sample.pdf = 1.0 / Area();
  sample.normal = glm::normalize(sample.p - center_);
}
bool Sphere::Intersect(const Ray& ray, Intersection& isect) const {
  auto oc = ray.o - center_;
  auto a = glm::dot(ray.d, ray.d);
  auto b = glm::dot(ray.d, oc);
  auto c = glm::dot(oc, oc) - radius_ * radius_;
  auto delta = b * b - 4 * a * c;
  if (delta < 0) {
    return false;
  }
  auto t1 = (-b - std::sqrt(delta)) / (2 * a);
  if (t1 >= ray.tmin) {
    if (t1 < isect.distance) {
      isect.distance = t1;
      auto p = ray.o + t1 * ray.d;
      isect.ng = glm::normalize(p - center_);
      isect.ns = isect.ng;
      isect.shape = this;
      return true;
    }
  }
  auto t2 = (-b - std::sqrt(delta)) / (2 * a);
  if (t2 >= ray.tmin) {
    if (t2 < isect.distance) {
      isect.distance = t2;
      auto p = ray.o + t2 * ray.d;
      isect.ng = glm::normalize(p - center_);
      isect.ns = isect.ng;
      isect.shape = this;
      return true;
    }
  }
  return false;
}
}  // namespace min::ray