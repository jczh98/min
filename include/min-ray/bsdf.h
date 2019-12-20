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

#include "spectrum.h"

namespace min::ray {

class ShadingPoint;
class BSDFSample;
class BSDF {
 public:
  enum Type : int {
    ENone = 0,
    EDiffuse = 1ULL,
    EGlossy = 1ULL << 1U,
    EReflection = 1ULL << 2U,
    ETransmission = 1ULL << 3U,
    ESpecular = 1ULL << 4U,
    EAll = EDiffuse | EGlossy | ETransmission | ESpecular | EReflection,
    EAllButSpecular = EAll & ~ESpecular
  };
  virtual Type GetBSDFType() const = 0;
  virtual Spectrum Evaluate(const Vector3 &wo, const Vector3 &wi) const = 0;
  virtual Float EvaluatePdf(const Vector3 &wo, const Vector3 &wi) const = 0;
  virtual void Sample(Point2 u, BSDFSample &sample) const = 0;
  virtual bool IsSpecular() const {
    return GetBSDFType() & ESpecular;
  }
};
struct BSDFSample {
  Vector3 wi;
  Vector3 wo;
  Float pdf;
  Spectrum s;
  BSDF::Type sample_type = BSDF::Type::ENone;
};
}  // namespace min::ray