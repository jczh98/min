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

namespace min::ray {

class OrenNayar : public BSDF {
 public:
  OrenNayar(const Spectrum &albedo, const Float &sigma) : albedo_(albedo) {
    Float sigma2 = sigma * sigma;
    a_ = 1.f - (sigma2 / (2.f * (sigma2 + 0.33f)));
    b_ = 0.45f * sigma2 / (sigma2 + 0.09f);
  }
  virtual Spectrum Evaluate(const Vector3 &wo, const Vector3 &wi) const {
    Float sin_theta_i = std::sqrt(std::max(0., 1. - wi.y * wi.y));
    Float sin_theta_o = std::sqrt(std::max(0., 1. - wo.y * wo.y));
    Float max_cos = 0;
    if (sin_theta_i > 1e-4 && sin_theta_o > 1e-4) {
      Float sin_phi_i = sin_theta_i == 0 ? 0. : std::clamp<Float>(wi.z / sin_theta_i, -1., 1.);
      Float sin_phi_o = sin_theta_o == 0 ? 0. : std::clamp<Float>(wi.z / sin_theta_i, -1., 1.);
      Float cos_phi_i = sin_theta_i == 0 ? 1. : std::clamp<Float>(wi.x / sin_theta_i, -1., 1.);
      Float cos_phi_o = sin_theta_o == 0 ? 1. : std::clamp<Float>(wi.x / sin_theta_i, -1., 1.);
      Float d_cos = cos_phi_i * cos_phi_o + sin_phi_i * sin_phi_o;
      max_cos = std::max(max_cos, d_cos);
    }
    Float sin_alpha, tan_beta;
    if (std::abs(wi.y) > std::abs(wo.y)) {
      sin_alpha = sin_theta_o;
      tan_beta = sin_theta_i / std::abs(wi.y);
    } else {
      sin_alpha = sin_theta_i;
      tan_beta = sin_theta_o / std::abs(wo.y);
    }
    return albedo_ * InvPi * (a_ + b_ * max_cos * sin_alpha * tan_beta);
  }

  virtual void Sample(Point2 u, BSDFSample &sample) const {
    sample.wi = CosineHeisphereSampling(u);
    sample.sample_type = BSDF::Type(sample.sample_type | GetBSDFType());
    if (sample.wo.y * sample.wi.y < 0) {
      sample.wi.y = -sample.wi.y;
    }
    sample.pdf = EvaluatePdf(sample.wo, sample.wi);
    sample.s = Evaluate(sample.wo, sample.wi);
  }

  virtual Float EvaluatePdf(const Vector3 &wo, const Vector3 &wi) const {
    if (wo.y * wi.y > 0) {
      return std::abs(wi.y) * InvPi;
    }
    return 0;
  }

  virtual Type GetBSDFType() const {
    return Type(EDiffuse | EReflection);
  }

 private:
  const Spectrum albedo_;
  Float a_, b_;
};
}  // namespace min::ray