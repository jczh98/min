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

#include "math.h"
#include "ray.h"
#include "texture.h"

namespace min::ray {

class Shape;

class BSDF;

class Primitive;

struct Intersection {
  void ComputeLocalFrame() {
    localframe = CoordinateSystem(ng);
    auto v = WorldToLocal(ng);
  }
  Vector3 LocalToWorld(const Vector3 &vec) const {
    return localframe.LocalToWorld(vec);
  }
  Vector3 WorldToLocal(const Vector3 &vec) const {
    return localframe.WorldToLocal(vec);
  }
  Ray SpawnRay(const Vector3 &w) {
    auto t = RayBias / abs(glm::dot(w, ng));
    return Ray(p, w, t, MaxFloat);
  }

  void ComputeScatteringFunctions();

  Ray SpawnTo(const Point3 &p) const {
    return Ray(this->p, (p - this->p), RayBias, 1);
  }
  const Shape *shape = nullptr;
  const Primitive *primitive = nullptr;
  const BSDF *bsdf = nullptr;
  ShadingPoint sp;
  Float distance = MaxFloat;
  Point3 p;
  Normal3 ns, ng;
  Point2 uv;
  CoordinateSystem localframe;
};
}  // namespace min::ray