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

#include <min-ray/camera.h>
#include <min-ray/integrator.h>
#include <min-ray/sampler.h>
#include <min-ray/scene.h>

namespace min::ray {

class AOIntegrator : public Integrator {
 public:
  AOIntegrator(int spp, Float occlude = 30) : spp_(spp), occlude_distance_(occlude) {}

  virtual void Render(const std::shared_ptr<Scene> &scene,
                      const std::shared_ptr<Camera> &camera,
                      const std::shared_ptr<Sampler> &sampler,
                      Film &film);

 private:
  int spp_;
  Float occlude_distance_ = 100000;
};
}  // namespace min::ray