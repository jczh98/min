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

#include "sahbvh.h"
#include <fmt/format.h>
#include <min-ray/mesh.h>

namespace min::ray {
class BVHAccelerator::BVHAcceleratorInternal {
 public:
  void Build(const std::vector<MeshTriangle> &primitives) {
    nodes_.clear();
    primitives_ = primitives;
    RecursiveBuild(0, primitives_.size(), 0);
    fmt::print("BVH nodes : {}\n", nodes_.size());
  }

  bool Intersect(const Ray &ray, Intersection &isect) const {
    bool hit = false;
    auto invd = Vector3(1) / ray.d;
    const int max_depth = 128;
    const BVHNode *stack[max_depth];
    int sp = 0;
    stack[sp++] = &nodes_[0];
    while (sp > 0) {
      auto p = stack[--sp];
      auto t = IntersectAABB(p->box, ray, invd);
      if (t < 0 || t > isect.distance) {
        continue;
      }
      if (p->IsLeaf()) {
        for (int i = p->first; i < p->first + p->count; i++) {
          if (primitives_[i].Intersect(ray, isect)) {
            hit = true;
          }
        }
      } else {
        if (p->left >= 0) {
          stack[sp++] = &nodes_[p->left];
        }
        if (p->right >= 0) {
          stack[sp++] = &nodes_[p->right];
        }
      }
    }
    return hit;
  }

  BoundingBox3 GetBoundingBox() const { return boundbox; }

 private:
  static Float IntersectAABB(const BoundingBox3 &box, const Ray &ray,
                             const Vector3 &invd) {
    Vector3 t0 = (box.pmin - ray.o) * invd;
    Vector3 t1 = (box.pmax - ray.o) * invd;
    Vector3 tmin = glm::min(t0, t1), tmax = glm::max(t0, t1);
    if (MaxComp(tmin) <= MinComp(tmax)) {
      auto t = std::max(ray.tmin + RayBias, MaxComp(tmin));
      if (t >= ray.tmax + RayBias) {
        return -1;
      }
      return t;
    }
    return -1;
  }
  int RecursiveBuild(int begin, int end, int depth) {
    BoundingBox3 box{{MaxFloat, MaxFloat, MaxFloat},
                     {MinFloat, MinFloat, MinFloat}};
    BoundingBox3 centroid{{MaxFloat, MaxFloat, MaxFloat},
                          {MinFloat, MinFloat, MinFloat}};
    if (depth == 0) {
      boundbox = box;
    }
    if (end == begin) return -1;
    for (int i = begin; i < end; i++) {
      box = box.UnionOf(primitives_[i].GetBoundingBox());
      centroid = centroid.UnionOf(primitives_[i].GetBoundingBox().Centroid());
    }
    if (end - begin <= 4 || depth >= 64) {
      BVHNode node;
      node.box = box;
      node.first = begin;
      node.count = end - begin;
      node.left = node.right = -1;
      nodes_.emplace_back(node);
      return nodes_.size() - 1;
    } else {
      int axis = depth % 3;
      auto size = centroid.Size();
      if (size.x > size.y) {
        if (size.x > size.z) {
          axis = 0;
        } else {
          axis = 2;
        }
      } else {
        if (size.y > size.z) {
          axis = 1;
        } else {
          axis = 2;
        }
      }
      const size_t num_buckets = 12;
      Bucket buckets[num_buckets];
      for (int i = begin; i < end; i++) {
        auto offset = centroid.offset(primitives_[i].GetBoundingBox().Centroid())[axis];
        int b = std::min<int>(num_buckets - 1, std::floor(offset * num_buckets));
        buckets[b].count++;
        buckets[b].bound.UnionOf(primitives_[i].GetBoundingBox());
      }
      Float cost[num_buckets - 1] = {};
      for (int i = 0; i < num_buckets - 1; i++) {
        BoundingBox3 b0, b1;
        int count0 = 0, count1 = 0;
        for (int j = 0; j <= i; j++) {
          b0 = b0.UnionOf(buckets[j].bound);
          count0 += buckets[j].count;
        }
        for (int j = i + 1; j < num_buckets; j++) {
          b1 = b1.UnionOf(buckets[j].bound);
          count1 += buckets[j].count;
        }
        cost[i] = 0.125 + (count0 * b0.Area() + count1 * b1.Area()) / box.Area();
      }
      int split_buckets = 0;
      Float min_cost = cost[0];
      for (int i = 1; i < num_buckets - 1; i++) {
        if (cost[i] <= min_cost) {
          min_cost = cost[i];
          split_buckets = i;
        }
      }
      auto mid = std::partition(
          &primitives_[begin], &primitives_[end - 1] + 1,
          [&](MeshTriangle &p) {
            int b = centroid.offset(p.GetBoundingBox().Centroid())[axis] * num_buckets;
            if (b == num_buckets) {
              b = num_buckets - 1;
            }
            return b <= split_buckets;
          });
      auto ret = nodes_.size();
      nodes_.emplace_back();
      BVHNode &node = nodes_.back();
      node.box = box;
      node.count = -1;
      nodes_.push_back(node);
      nodes_[ret].left = RecursiveBuild(begin, mid - &primitives_[0], depth + 1);
      nodes_[ret].right = RecursiveBuild(mid - &primitives_[0], end, depth + 1);
      return ret;
    }
  }
  struct BVHNode {
    BoundingBox3 box;
    uint32_t first = -1;
    uint32_t count = -1;
    int left = -1, right = -1;
    bool IsLeaf() const { return left < 0 && right < 0; }
  };
  struct Bucket {
    size_t count = 0;
    BoundingBox3 bound;
    Bucket() : bound{{MaxFloat, MaxFloat, MaxFloat},
                     {MinFloat, MinFloat, MinFloat}} {}
  };
  std::vector<MeshTriangle> primitives_;
  std::vector<BVHNode> nodes_;
  BoundingBox3 boundbox;
};

void BVHAccelerator::Build(Scene &scene) {
  for (const auto &i : scene.meshes()) {
    auto node = new BVHAcceleratorInternal();
    node->Build(i->triangles);
    internal_.emplace_back(node);
  }
}

bool BVHAccelerator::Intersect(const Ray &ray, Intersection &isect) {
  bool hit = false;
  for (auto i : internal_) {
    if (i->Intersect(ray, isect)) {
      hit = true;
    }
  }
  if (hit) {
    isect.p = isect.distance * ray.d + ray.o;
  }
  return hit;
}

BVHAccelerator::~BVHAccelerator() {
  for (auto i : internal_) {
    delete i;
  }
}

}  // namespace min::ray