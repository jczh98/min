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
#include <min-ray/logging.h>

namespace min::ray {

class Bruteforce : public Accelerator {
  std::vector<std::shared_ptr<Primitive>> primitives;
 public:
  Bruteforce(const std::vector<std::shared_ptr<Primitive>> &primitives) : primitives(primitives) {
    fmt::print("primitives : {}\n", primitives.size());
  }
  virtual bool Intersect(const Ray &ray, Intersection &isect) const {
		bool hit = false;
    for (auto pri : primitives) {
      if (pri->Intersect(ray, isect)) {
        hit = true;
      }
		}
		return hit;
	}
  virtual BoundingBox3 GetBoundingBox() const {
    MIN_NOT_IMPLEMENTED
    return BoundingBox3();
  }
  virtual void Sample(const Point2 &p, SurfaceSample &sample) const {
    MIN_NOT_IMPLEMENTED
        }
  virtual Float Area() const {
    MIN_NOT_IMPLEMENTED
    return 0; 
		}
};
}  // namespace min::ray