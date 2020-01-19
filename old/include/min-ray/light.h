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

#include "intersection.h"
#include "ray.h"
#include "spectrum.h"

namespace min::ray {

class MeshTriangle;

struct VisibilityTester;
struct ShadingPoint;

struct LightSample {
  Vector3 wi;
  Spectrum li;
  float pdf;
};

struct LightRaySample {
  Ray ray;
  Spectrum le;
  float pdf_pos, pdf_dir;
};

class Light {
 public:
  virtual Spectrum Li(ShadingPoint &sp) const = 0;

  virtual void SampleLi(const Point2 &u, Intersection &isect, LightSample &sample, VisibilityTester &test) const = 0;

  virtual Float PdfLi(const Intersection &isect, const Vector3 &wi) const = 0;

  virtual void SampleLe(const Point2 &u1, const Point2 &u2, LightRaySample &sample) = 0;
};

class AreaLight : public Light {
 public:
  virtual void SetTriangle(MeshTriangle *shape) = 0;
};

}  // namespace min::ray