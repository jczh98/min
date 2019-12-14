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

#include "intersection.h"
#include "material.h"
#include "ray.h"
#include "shape.h"

namespace min::ray {
class Mesh;
class Light;
struct VertexData {
  std::vector<Point3> positions;
  std::vector<Normal3> normals;
  std::vector<Point2> tex_coords;
};
struct MeshTriangle {
  const Point3 &vertex(size_t) const;
  const Normal3 &normal(size_t) const;
  const Point2 &texCoord(size_t) const;
  bool Intersect(const Ray &ray, Intersection &isect) const {
    Vector3 e1 = (vertex(1) - vertex(0));
    Vector3 e2 = (vertex(2) - vertex(0));
    auto ng = glm::normalize(glm::cross(e1, e2));
    Float a, f, u, v;
    auto h = glm::cross(ray.d, e2);
    a = dot(e1, h);
    if (a > -1e-6f && a < 1e-6f)
      return false;
    f = 1.0f / a;
    auto s = ray.o - vertex(0);
    u = f * dot(s, h);
    if (u < 0.0 || u > 1.0)
      return false;
    auto q = cross(s, e1);
    v = f * dot(ray.d, q);
    if (v < 0.0 || u + v > 1.0)
      return false;
    float t = f * dot(e2, q);
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
  BoundingBox3 getBoundingBox() const {
    return BoundingBox3{std::min(vertex(0), std::min(vertex(1), vertex(2))), std::max(vertex(0), std::max(vertex(1), vertex(2)))};
  }
  Light *light = nullptr;
  Mesh *mesh = nullptr;
  uint16_t name_id = -1;
};

class Mesh {
 public:
  bool LoadFromFile(const std::string &filename) const;
  bool loaded = false;
  std::vector<MeshTriangle> triangles;
  VertexData vertex_data;
  std::vector<Point3> indices;
  std::vector<std::shared_ptr<Material>> materials;
  std::unordered_map<std::string, std::shared_ptr<Material>> named_materials;
  std::vector<std::string> names;
  std::string filename;
};

}  // namespace min::ray