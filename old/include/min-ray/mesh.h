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
#include "interface.h"
#include "material.h"
#include "shape.h"

namespace min::ray {
class Mesh;

class AreaLight;

struct VertexData {
  std::vector<Point3> position;
  std::vector<Normal3> normal;
  std::vector<Point2> texcoord;
};

struct VertexIndices {
  Point3i position;
  Point3i normal;
  Point3i texcoord;
};

class Light;

struct MeshTriangle {
  VertexIndices indices;
  Light *light = nullptr;
  Mesh *mesh = nullptr;
  uint16_t name_id = -1;

  MeshTriangle() = default;

  inline const Point3 &vertex(size_t) const;

  inline Normal3 normal(size_t) const;

  inline Point2 texcoord(size_t) const;

  Vector3 NormalGeometry() const {
    auto v0 = vertex(0);
    Vector3 e1 = (vertex(1) - v0);
    Vector3 e2 = (vertex(2) - v0);
    return normalize(cross(e1, e2));
  }

  bool Intersect(const Ray &ray, Intersection &isct) const {
    Vector3 e1 = (vertex(1) - vertex(0));
    Vector3 e2 = (vertex(2) - vertex(0));
    auto ng = normalize(cross(e1, e2));
    float a, f, u, v;
    auto h = cross(ray.d, e2);
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
      if (t < isct.distance && t < ray.tmax) {
        isct.distance = t;
        isct.ng = ng;
        isct.uv = Point2(u, v);
        isct.mesh = this;
        return true;
      }
      return false;
    } else {
      return false;
    }
  }

  BoundingBox3 GetBoundingBox() const {
    return BoundingBox3{glm::min(vertex(0), glm::min(vertex(1), vertex(2))), glm::max(vertex(0), glm::max(vertex(1), vertex(2)))};
  }

  void Sample(const Point2 &u, SurfaceSample &sample) const {
    Point2 uv = u;
    if (uv.x + uv.y > 1.0f) {
      uv.x = 1.0f - uv.x;
      uv.y = 1.0f - uv.y;
    }
    sample.uv = uv;
    sample.pdf = 1.0f / Area();
    sample.p = PositionAt(sample.uv);
    sample.normal = NormalGeometry();
  }

  Float Area() const {
    return length(cross(Vector3(vertex(1) - vertex(0)), (vertex(2) - vertex(0))));
  }

  BSDF *GetBSDF() const { return nullptr; }

  Material *GetMaterial() const;

  Vector3 PositionAt(const Point2 &uv) const {
    return Lerp3(vertex(0), vertex(1), vertex(2), uv[0], uv[1]);
  }

  Normal3 NormalAt(const Point2 &uv) const {
    return normalize(Lerp3(normal(0), normal(1), normal(2), uv[0], uv[1]));
  }

  Point2 TexcoordAt(const Point2 &uv) const {
    return Lerp3(texcoord(0), texcoord(1), texcoord(2), uv[0], uv[1]);
  }
};

class Mesh {
 public:
  bool loaded = false;
  std::vector<MeshTriangle> triangles;
  VertexData vertex_data;
  std::vector<std::string> names;
  std::vector<std::shared_ptr<Material>> materials;
  std::unordered_map<std::string, std::shared_ptr<Material>> materials_map;
  std::string filename;

  void Foreach (const std::function<void(MeshTriangle *)> &func) {
    for (auto &i : triangles) {
      func(&i);
    }
  }

  virtual void Preprocess(){
    materials.clear();
    for (const auto &name : names) {
      if (materials_map.find(name) != materials_map.end()) {
        auto mat = materials_map.at(name);
        materials.emplace_back(mat);
      } else {
        materials.emplace_back(nullptr);
      }
    }
  }
};

class MeshInstance final : public Mesh {
 public:

  TransformManipulator transform;
  std::shared_ptr<Mesh> mesh;

  void Foreach (const std::function<void(MeshTriangle *)> &func) { mesh->Foreach (func); }

 protected:
  void Preprocess() override { MIN_NOT_IMPLEMENTED }
};
inline const Point3 &MeshTriangle::vertex(size_t i) const {
  return mesh->vertex_data.position[indices.position[i]];
}

inline Normal3 MeshTriangle::normal(size_t i) const {
  auto idx = indices.normal[i];
  return idx >= 0 ? mesh->vertex_data.normal[idx] : NormalGeometry();
}

inline Point2 MeshTriangle::texcoord(size_t i) const {
  auto idx = indices.texcoord[i];
  return idx >= 0 ? mesh->vertex_data.texcoord[idx] : Point2(i > 0, i > 1);
}

inline Material *MeshTriangle::GetMaterial() const {
  return name_id >= 0 ? mesh->materials[name_id].get() : nullptr;
}
}  // namespace min::ray