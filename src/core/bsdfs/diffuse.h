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
#include <min-ray/sampling.h>
#include <min-ray/shader.h>

namespace min::ray {

class DiffuseBSDF : public BSDF {
 public:
  DiffuseBSDF(const std::shared_ptr<Shader> &shader) : shader_(shader) {}
  virtual Spectrum Evaluate(const ShadingPoint &sp, const Vector3 &wo, const Vector3 &wi) const {
    if (wo.y * wi.y > 0) {
      return shader_->Evaluate(sp) * InvPi;
    }
    return Spectrum(0);
  }
  virtual void Sample(Point2 u, const ShadingPoint &sp, BSDFSample &sample) const {
    sample.wi = CosineHeisphereSampling(u);
    sample.sample_type = BSDF::Type(sample.sample_type | GetBSDFType());
    if (sample.wo.y * sample.wi.y < 0) {
      sample.wi.y = -sample.wi.y;
    }
    sample.pdf = std::abs(sample.wi.y) * InvPi;
    sample.s = Evaluate(sp, sample.wo, sample.wi);
  }

  virtual Float EvaluatePdf(const ShadingPoint &sp, const Vector3 &wo, const Vector3 &wi) const {
    if (wo.y * wi.y > 0) {
      return std::abs(wi.y) * InvPi;
    }
    return 0;
  }

  virtual Type GetBSDFType() const {
    return Type(EDiffuse | EReflection);
  }

 private:
  std::shared_ptr<Shader> shader_;
};
}  // namespace min::ray