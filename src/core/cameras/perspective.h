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
#include <glm/gtc/matrix_transform.hpp>

namespace min::ray {

class PerspectiveCamera : public Camera {
 public:
  PerspectiveCamera() = default;
  PerspectiveCamera(const Vector3 &eye, const Vector3 &center, Float fov) : fov_(fov) {
    transform_ = Transform(glm::lookAt(eye, center, Vector3(0, 1, 0)));
    inv_transform_ = transform_.Inverse();
  }
  virtual void GenerateRay(const Point2 &eye,
                           const Point2 &center,
                           const Point2i &raster,
                           Point2i dimension,
                           CameraSample &sample) const;

 private:
  Transform transform_, inv_transform_;
  Float fov_;
};
}  // namespace min::ray