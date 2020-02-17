#pragma once

#include <min/visual/accel.h>
#include <atomic>

namespace min {
struct BVHBuildNode;

// BVHAccel Forward Declarations
struct BVHPrimitiveInfo;
struct MortonPrimitive;
struct LinearBVHNode;

// BVHAccel Declarations
class BVHAccel : public Accelerator {
 public:
  // BVHAccel Public Types
  enum class SplitMethod { SAH, HLBVH, Middle, EqualCounts };
  void initialize(const Json &json) override;
  Bounds3f WorldBound() const override ;
  ~BVHAccel();
  bool Intersect(const Ray &ray, SurfaceIntersection &isect) const override;
  bool IntersectP(const Ray &ray) const override;
  void AddShape(const std::vector<std::shared_ptr<Shape>> &shape) override;
  void Build() override;
 private:
  // BVHAccel Private Methods
  BVHBuildNode *recursiveBuild(
      std::vector<BVHPrimitiveInfo> &primitiveInfo,
      int start, int end, int *totalNodes,
      std::vector<std::shared_ptr<Shape>> &orderedPrims);
  BVHBuildNode *HLBVHBuild(
      const std::vector<BVHPrimitiveInfo> &primitiveInfo,
      int *totalNodes,
      std::vector<std::shared_ptr<Shape>> &orderedPrims) const;
  BVHBuildNode *emitLBVH(
      BVHBuildNode *&buildNodes,
      const std::vector<BVHPrimitiveInfo> &primitiveInfo,
      MortonPrimitive *mortonPrims, int nPrimitives, int *totalNodes,
      std::vector<std::shared_ptr<Shape>> &orderedPrims,
      std::atomic<int> *orderedPrimsOffset, int bitIndex) const;
  BVHBuildNode *buildUpperSAH(std::vector<BVHBuildNode *> &treeletRoots,
                              int start, int end, int *totalNodes) const;
  int flattenBVHTree(BVHBuildNode *node, int *offset);

  // BVHAccel Private Data
  int maxPrimsInNode;
  SplitMethod splitMethod;
  std::vector<std::shared_ptr<Shape>> primitives;
  LinearBVHNode *nodes = nullptr;
};

}


