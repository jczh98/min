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
void Triangle::get_coord(const Vector3 &inter, Float &coord_u, Float &coord_v) const{
  const Point3 &p0 = mesh_->positions[v_[0]];
  const Point3 &p1 = mesh_->positions[v_[1]];
  const Point3 &p2 = mesh_->positions[v_[2]];
  const Vector3 inter_local = inter - p0;
  const Vector3 u = p1-p0, v = p2-p0;
  Float uv = dot(u, v), vv = dot(v, v), wu = dot(inter_local, u),
        uu = dot(u, u), wv = dot(inter_local, v);
  Float dom = uv * uv - uu * vv;
  coord_u = (uv * wv - vv * wu) / dom;
  coord_v = (uv * wu - uu * wv) / dom;
}
bool Triangle::Intersect(const Ray &ray, Intersection &isect) const {
  const Point3 &p0 = mesh_->positions[v_[0]];
  const Point3 &p1 = mesh_->positions[v_[1]];
  const Point3 &p2 = mesh_->positions[v_[2]];
  Vector3 e1 = (p1 - p0);
  Vector3 e2 = (p2 - p0);
  auto ng = glm::normalize(glm::cross(e1, e2));

  Float dir_n = dot(ray.d, ng);
  Float dist_n = dot(p0 - ray.o, ng);
  Float dist = dist_n / dir_n;
  if (dist <= 0.0f) {
    return false;
  } else {
    if (dist > ray.tmin && dist < isect.distance) {
      Float coord_u, coord_v;
      get_coord(ray.o + ray.d * dist, coord_u, coord_v);
      if (coord_u >= 0 && coord_v >= 0 && coord_u + coord_v <= 1) {
        isect.distance = dist;
        isect.uv = Point2(coord_u, coord_v);
        isect.ng = ng;
        isect.shape = this;
        return true;
      }
    }
    return false;
  }

  // float a, f, u, v;
  // auto h = cross(ray.d, e2);
  // a = dot(e1, h);
  // if (a > -1e-6f && a < 1e-6f)
  //   return false;
  // f = 1.0f / a;
  // auto s = ray.o - p0;
  // u = f * dot(s, h);
  // if (u < 0.0 || u > 1.0)
  //   return false;
  // auto q = cross(s, e1);
  // v = f * dot(ray.d, q);
  // if (v < 0.0 || u + v > 1.0)
  //   return false;
  // float t = f * dot(e2, q);
  // if (t > ray.tmin) {
  //   if (t < isect.distance) {
  //     isect.distance = t;
  //     isect.ng = ng;
  //     isect.uv = Point2(u, v);
  //     isect.shape = this;
  //     return true;
  //   }
  //   return false;
  // } else {
  //   return false;
  // }
}

void Triangle::Sample(const Point2 &p, SurfaceSample &sample) const {
}
}  // namespace min::ray