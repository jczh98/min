#pragma once

#include <min-ray/mesh.h>

namespace min::ray {

class Accel {
  friend class BVHBuildTask;
 public:
  Accel() {
    mesh_offset.push_back(0u);
  }
  ~Accel() {
    meshes.clear();
    mesh_offset.clear();
    mesh_offset.push_back(0u);
    nodes.clear();
    indices.clear();
    bbox.Reset();
    nodes.shrink_to_fit();
    meshes.shrink_to_fit();
    mesh_offset.shrink_to_fit();
    indices.shrink_to_fit();
  }

  void AddMesh(const std::shared_ptr<Mesh> &mesh);

  void Build();

  const BoundingBox3f &GetBoundingBox() const { return bbox; }

  bool Intersect(const Ray3f &ray, Intersection &its, bool shadowRay) const;

 private:
  struct BVHNode {
    union {
      struct {
        unsigned flag : 1;
        uint32_t size : 31;
        uint32_t start;
      } leaf;

      struct {
        unsigned flag : 1;
        uint32_t axis : 31;
        uint32_t rightChild;
      } inner;

      uint64_t data;
    };
    BoundingBox3f bbox;

    bool isLeaf() const {
      return leaf.flag == 1;
    }

    bool isInner() const {
      return leaf.flag == 0;
    }

    bool isUnused() const {
      return data == 0;
    }

    uint32_t start() const {
      return leaf.start;
    }

    uint32_t end() const {
      return leaf.start + leaf.size;
    }
  };

  std::pair<float, uint32_t> statistics(uint32_t index = 0) const;

  uint32_t FindMesh(uint32_t &idx) const {
    auto it = std::lower_bound(mesh_offset.begin(), mesh_offset.end(), idx+1) - 1;
    idx -= *it;
    return (uint32_t) (it - mesh_offset.begin());
  }

  Point3f GetCentroid(uint32_t index) const {
    uint32_t shapeIdx = FindMesh(index);
    return meshes[shapeIdx]->centroid(index);
  }

  BoundingBox3f GetBoundingBox(uint32_t index) const {
    uint32_t shapeIdx = FindMesh(index);
    return meshes[shapeIdx]->GetBoundingBox(index);
  }

  std::vector<std::shared_ptr<Mesh>> meshes;
  std::vector<uint32_t> mesh_offset;
  std::vector<BVHNode> nodes;
  std::vector<uint32_t> indices;
  BoundingBox3f bbox;
};

}  // namespace min::ray
