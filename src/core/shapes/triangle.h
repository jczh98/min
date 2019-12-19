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

#include <min-ray/shape.h>

namespace min::ray {
struct MeshTriangle {
  std::vector<Point3> positions;
  std::vector<Normal3> normals;
  std::vector<Point2> tex_coords;
  std::vector<int> vertex_indices;
  std::vector<int> face_indices;
};
class Triangle : public Shape {
 public:
  Triangle(const std::shared_ptr<MeshTriangle> &mesh, int tri_number) : mesh_(mesh) {
    v_ = &mesh_->vertex_indices[3 * tri_number];
    face_index_ = mesh_->face_indices.size() ? mesh_->face_indices[tri_number] : 0;
  }
  bool Intersect(const Ray &ray, Intersection &isect) const override;
  void Sample(const Point2 &, SurfaceSample &) const override;
  BoundingBox3 GetBoundingBox() const override {
    const Point3 &p0 = mesh_->positions[v_[0]];
    const Point3 &p1 = mesh_->positions[v_[1]];
    const Point3 &p2 = mesh_->positions[v_[2]];
    const BoundingBox3 box{p0, p1};
    return box.UnionOf(p2);
  }
  Float Area() const override {
    const Point3 &p0 = mesh_->positions[v_[0]];
    const Point3 &p1 = mesh_->positions[v_[1]];
    const Point3 &p2 = mesh_->positions[v_[2]];
    return 0.5 * glm::length(glm::cross(p1 - p0, p2 - p0));
  }

 private:
  std::shared_ptr<MeshTriangle> mesh_;
  const int *v_;
  int face_index_;
};
}  // namespace min::ray