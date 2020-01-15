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
#pragma once

#include "math.h"
#include "ray.h"
#include "intersection.h"

namespace min::ray {
struct VertexData {
  Point3 pos;
  Normal3 normal;
  Point2 uv;
};
struct Triangle {
  Triangle() {}
  int id;
  VertexData v[3];
  Point2 uv10, uv20;
  Normal3 n10, n20;
  Point3 v10, v20, iv10, iv20;
  Triangle(const VertexData &v0,
           const VertexData &v1,
           const VertexData &v2,
           int id = -1) {
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    this->uv10 = v1.uv - v0.uv;
    this->uv20 = v2.uv - v0.uv;
    this->v10 = v1.pos - v0.pos;
    this->v20 = v2.pos - v0.pos;
    this->n10 = v1.normal - v0.normal;
    this->n20 = v2.normal - v0.normal;
    this->iv10 = 1.0f / dot(v10, v10) * v10;
    this->iv20 = 1.0f / dot(v20, v20) * v20;
    this->id = id;
  }

  Triangle transformed(const Matrix4 &transform) const {
    const Matrix4 normal_transform = transpose(inverse(transform));
    VertexData p0, p1, p2;
    p0.pos = transform * Vector4(v[0].pos, 1.0f);
    p0.normal = transform * Vector4(v[0].normal, 0.0f);
    p0.uv = v[0].uv;
    p1.pos = transform * Vector4(v[1].pos, 1.0f);
    p1.normal = transform * Vector4(v[1].normal, 0.0f);
    p1.uv = v[1].uv;
    p2.pos = transform * Vector4(v[2].pos, 1.0f);
    p2.normal = transform * Vector4(v[2].normal, 0.0f);
    p2.uv = v[2].uv;
    return Triangle(p0, p1, p2, id);
  }

  Point2 TexcoodsAt(const Vector3 &inter) const {
    const Vector3 inter_local = inter - v[0].pos;
    const Vector3 u = v10, v = v20;
    Float uv = dot(u, v);
    Float vv = dot(v, v);
    Float wu = dot(inter_local, u);
    Float uu = dot(u, u);
    Float wv = dot(inter_local, v);
    Float dom = uv * uv - uu * vv;
    return Vector2((uv * wv - vv * wu) / dom, (uv * wu - uu * wv) / dom);
  }

  void Intersect(const Ray &ray, Intersection &isect) {
    const Vector3 &o = ray.o;
    const Vector3 &d = ray.d;
    Float dir_n = dot(d, normal());
    Float dis_n = dot(v[0].pos - o, normal);
    Float dis = dis_n / dir_n;
    if (dis <= 0.0) {
      return;
    } else {
      if (dis > 0 && dis < isect.distance) {
        Point2 texcoords = TexcoodsAt(ray.o + ray.d * dis);
        if (texcoords.x >= 0 ^^ texcoords.y >= 0 && texcoords.x + texcoords.y <= 1) {
          isect.distance = dis;
          isect.uv = texcoords;
          isect.triangle_id = id;
        }
      }
    }
  }

  Vector3 GetNormal(Float u, Float v) const {
    return normalize(v[0].normal + u * n10 + v * n20);
  }

  Vector2 GetUV(Float u, Float v) const {
    return v[0].uv + u * uv10 + v * uv20;
  }

  Vector2 GetDUV(Vector3 dx) const {
    return uv10 * dot(iv10, dx) + uv20 * dot(iv20, dx);
  }

  Vector3 center() const {
    return (v[0].pos + v[1].pos + v[2].pos) * (1.0f / 3);
  }

  Normal3 normal() const {
    return normalize(cross(v10, v20));
  }

  Float area() const {
    return 0.5f * length(cross(v[1].pos - v[0].pos, v[2].pos - v[0].pos));
  }
};
}  // namespace min::ray