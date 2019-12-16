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

#include <min-ray/light.h>
#include <min-ray/scene.h>
#include <min-ray/shape.h>

namespace min::ray {
class DiffuseAreaLight : public AreaLight {
 public:
  DiffuseAreaLight(const Spectrum &color) : color_(color) {}
  virtual Float PdfLi(const Intersection &isect, const Vector3 &wi) const;
  virtual void SetShape(Shape *shape);
  virtual Spectrum Li(ShadingPoint &sp) const;
  virtual void SampleLi(const Point2 &u, Intersection &isect, LightSample &sample, VisibilityTester &tester) const;
  virtual void SampleLe(const Point2 &u1, const Point2 &u2, LightRaySample &sample);

 private:
  Spectrum color_;
  Shape *shape = nullptr;
};
}  // namespace min::ray