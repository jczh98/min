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

#include "triangle.h"

namespace min::ray {
bool Triangle::Intersect(const Ray &ray, Intersection &isect) const {
  const Point3 &p0 = mesh_->positions[v_[0]];
  const Point3 &p1 = mesh_->positions[v_[1]];
  const Point3 &p2 = mesh_->positions[v_[2]];
  Vector3 e1 = (p1 - p0);
  Vector3 e2 = (p2 - p0);
  auto ng = glm::normalize(glm::cross(e1, e2));
  Float a, f, u, v;
  auto h = glm::cross(ray.d, e2);
  a = dot(e1, h);
  if (a > -1e-6f && a < 1e-6f)
    return false;
  f = 1.0f / a;
  auto s = ray.o - p0;
  u = f * glm::dot(s, h);
  if (u < 0.0 || u > 1.0)
    return false;
  auto q = glm::cross(s, e1);
  v = f * glm::dot(ray.d, q);
  if (v < 0.0 || u + v > 1.0)
    return false;
  float t = f * glm::dot(e2, q);
  if (t > ray.tmin) {
    if (t < isect.distance) {
      isect.distance = t;
      isect.ng = ng;
      isect.uv = Point2(u, v);
      isect.shape = this;
      return true;
    }
    return false;
  } else {
    return false;
  }
}

void Triangle::Sample(const Point2 &p, SurfaceSample &sample) const {
}
}  // namespace min::ray