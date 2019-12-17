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

namespace min::ray {

class PerspectiveCamera : public Camera {
 public:
  static Matrix4 LookAt(const Vector3 &from, const Vector3 &to) {
    Vector3 up(0, 1, 0);
    Vector3 d = glm::normalize(to - from);
    Vector3 xAxis = glm::normalize(glm::cross(up, d));
    Vector3 yAxis = glm::normalize(glm::cross(xAxis, d));
    Matrix4 Result = {
        {xAxis.x, yAxis.x, d.x, 0},
        {xAxis.y, yAxis.y, d.y, 0},
        {xAxis.z, yAxis.z, d.z, 0},
        {0, 0, 0, 1}};
    return Result;
  }
  PerspectiveCamera() = default;
  PerspectiveCamera(const Vector3 &eye, const Vector3 &center, Float fov) : fov_(fov) {
    viewpoint_ = eye;
    transform_ = LookAt(eye, center);
    inv_transform_ = glm::inverse(transform_);
  }

  virtual Vector3 WorldToCamera(const Vector3 &v) const {
    auto r = inv_transform_ * Vector4(v.x, v.y, v.z, 1);
    return {r.x, r.y, r.z};
  }
  virtual Vector3 CameraToWorld(const Vector3 &v) const {
    auto r = transform_ * Vector4(v.x, v.y, v.z, 1);
    return {r.x, r.y, r.z};
  }
  virtual void GenerateRay(const Point2 &u1,
                           const Point2 &u2,
                           const Point2i &raster,
                           Point2i dimension,
                           CameraSample &sample) const;

 private:
  Matrix4 transform_, inv_transform_;
  Vector3 viewpoint_;
  Radians<float> fov_ = Radians<float>(Degrees<float>(80.0));
};
}  // namespace min::ray