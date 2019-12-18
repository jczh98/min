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

#include "accelerator.h"
#include "intersection.h"
#include "light.h"
#include "primitive.h"
#include "ray.h"
#include "sampler.h"

namespace min::ray {
class Scene {
 public:
  void Preprocess();
  bool Intersect(const Ray &ray, Intersection &isect);
  size_t GetRayCounter() const { return ray_counter_; }
  std::vector<std::shared_ptr<Primitive>> &primitives() {
    return primitives_;
  }
  Light *SampleLight(const std::shared_ptr<Sampler> &sampler) const;

  std::shared_ptr<Accelerator> &accelerator() { return accelerator_; }
  std::vector<Light *> &lights() { return lights_; }

 private:
  std::atomic<size_t> ray_counter_ = 0;
  std::shared_ptr<Accelerator> accelerator_;
  std::vector<std::shared_ptr<Primitive>> primitives_;
  std::vector<Light *> lights_;
};

struct VisibilityTester {
  bool Visible(const std::shared_ptr<Scene> &scene) {
    Intersection isect;
    if (!scene->Intersect(shadow_ray, isect) || isect.distance >= shadow_ray.tmax - RayBias) {
      return true;
    }
    return false;
  }
  Ray shadow_ray;
};
}  // namespace min::ray