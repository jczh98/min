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

#include <min-ray/accelerator.h>

namespace min::ray {

class BVHAccelerator : public Accelerator {
 public:
  BVHAccelerator(const std::vector<std::shared_ptr<Primitive>> &primitives) : primitives_(primitives) {
    RecursiveBuild(0, static_cast<int>(primitives_.size()), 0);
    fmt::print("BVH nodes : {}\n", nodes_.size());
  }
  virtual bool Intersect(const Ray &ray, Intersection &isect) const;
  virtual BoundingBox3 GetBoundingBox() const {
    return boundbox_;
  }
  virtual void Sample(const Point2 &p, SurfaceSample &sample) const;
  virtual Float Area() const { return 0; }

 private:
  struct BVHNode {
    BoundingBox3 box;
    uint32_t first, count;
    int left, right;
    bool IsLeaf() const {
      return left < 0 && right < 0;
    }
  };
  struct Bucket {
    size_t count = 0;
    BoundingBox3 bound;
    Bucket() : bound{{MaxFloat, MaxFloat, MaxFloat},
                     {MinFloat, MinFloat, MinFloat}} {}
  };
  int RecursiveBuild(int begin, int end, int depth);
  std::vector<std::shared_ptr<Primitive>> primitives_;
  std::vector<BVHNode> nodes_;
  BoundingBox3 boundbox_;
};
}  // namespace min::ray