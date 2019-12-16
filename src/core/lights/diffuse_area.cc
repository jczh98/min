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
#include "diffuse_area.h"

namespace min::ray {
Float DiffuseAreaLight::PdfLi(const Intersection &isect, const Vector3 &wi) const {
  Intersection isect_;
  Ray ray(isect.p, wi, RayBias);
  if (!shape->Intersect(ray, isect_)) {
    return 0.0f;
  }
  Float SA = shape->Area() * std::abs(glm::dot(wi, isect_.ng)) / (isect_.distance * isect_.distance);
  return 1.0f / SA;
}

void DiffuseAreaLight::SetShape(Shape *shape) {
  this->shape = shape;
}

Spectrum DiffuseAreaLight::Li(ShadingPoint &sp) const {
  return color_;
}

void DiffuseAreaLight::SampleLi(const Point2 &u, Intersection &isect, LightSample &sample, VisibilityTester &tester) const {
  SurfaceSample surface_sample;
  shape->Sample(u, surface_sample);
  auto wi = surface_sample.p - isect.p;
  tester.shadow_ray = Ray(isect.p, wi, RayBias, 1);
  sample.li = color_;
  sample.wi = glm::normalize(wi);
  sample.pdf = glm::dot(wi, wi) / std::abs(glm::dot(surface_sample.normal, sample.wi)) * surface_sample.pdf;
}

void DiffuseAreaLight::SampleLe(const Point2 &u1, const Point2 &u2, LightRaySample &sample) {
}
}  // namespace min::ray