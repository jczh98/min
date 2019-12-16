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

class Sphere : public Shape {
 public:
  Sphere(Float r, const Vector3 &center, const std::shared_ptr<BSDF> &bsdf, const std::shared_ptr<AreaLight> &light = nullptr)
      : radius_(r), center_(center), bsdf_(bsdf), light_(light) {
    if (light) {
      light->SetShape(this);
    }
  }
  virtual BSDF *GetBSDF() const {
    return bsdf_.get();
  }

  virtual void Sample(const Point2 &, SurfaceSample &) const;
  virtual Float Area() const {
    return 4 * Pi * radius_ * radius_;
  }
  virtual bool Intersect(const Ray &ray, Intersection &isect) const;
  virtual BoundingBox3 GetBoundingBox() const {
    return BoundingBox3{center_ - radius_, center_ + radius_};
  }

 private:
  std::shared_ptr<BSDF> bsdf_;
  Float radius_;
  Vector3 center_;
  std::shared_ptr<AreaLight> light_;
};
}  // namespace min::ray