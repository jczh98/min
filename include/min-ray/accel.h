
#pragma once

#include <min-ray/mesh.h>

namespace min::ray {

/**
 * \brief Acceleration data structure for ray intersection queries
 *
 * The current implementation falls back to a brute force loop
 * through the geometry.
 */
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
  /**
     * \brief Register a triangle mesh for inclusion in the acceleration
     * data structure
     *
     * This function can only be used before \ref build() is called
     */
  void AddMesh(const std::shared_ptr<Mesh> &mesh);

  /// Build the acceleration data structure (currently a no-op)
  void Build();

  /// Return an axis-aligned box that bounds the scene
  const BoundingBox3f &GetBoundingBox() const { return bbox; }

  /**
     * \brief Intersect a ray against all triangles stored in the scene and
     * return detailed intersection information
     *
     * \param ray
     *    A 3-dimensional ray data structure with minimum/maximum extent
     *    information
     *
     * \param its
     *    A detailed intersection record, which will be filled by the
     *    intersection query
     *
     * \param shadowRay
     *    \c true if this is a shadow ray query, i.e. a query that only aims to
     *    find out whether the ray is blocked or not without returning detailed
     *    intersection information.
     *
     * \return \c true if an intersection was found
     */
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
  //Mesh *mesh = nullptr;  ///< Mesh (only a single one for now)
  BoundingBox3f bbox;    ///< Bounding box of the entire scene
};

}  // namespace min::ray
