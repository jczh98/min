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

#include <min-ray/bsdf.h>
#include <min-ray/texture.h>

namespace min::ray {
class DiffuseBSDF : public BSDF {
  std::shared_ptr<Texture> color;

 public:
  DiffuseBSDF() = default;
  explicit DiffuseBSDF(const std::shared_ptr<Texture> &color) : color(color) {}

  Spectrum Evaluate(const ShadingPoint &sp, const Vector3 &wo, const Vector3 &wi) const override;

  void Sample(Point2 u, const ShadingPoint &sp, BSDFSample &sample) const override;

  Float EvaluatePdf(const ShadingPoint &point, const Vector3 &wo, const Vector3 &wi) const override;

  Type GetBSDFType() const override {
    return Type(EDiffuse | EReflection);
  }
};
}  // namespace min::ray