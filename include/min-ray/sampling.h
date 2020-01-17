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

namespace min::ray {

inline Point2 ConcentricSampleDisk(const Point2 &u) {
  // Map uniform random numbers to $[-1,1]^2$
  Point2 uOffset = 2.f * u - Vector2(1, 1);

  // Handle degeneracy at the origin
  if (uOffset.x == 0 && uOffset.y == 0) return Point2(0, 0);

  // Apply concentric mapping to point
  Float theta, r;
  if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
    r = uOffset.x;
    theta = Pi4 * (uOffset.y / uOffset.x);
  } else {
    r = uOffset.y;
    theta = Pi2 - Pi4 * (uOffset.x / uOffset.y);
  }
  return r * Point2(std::cos(theta), std::sin(theta));
}

inline Vector3 CosineHemisphereSampling(const Point2 &u) {
  Point2 d = ConcentricSampleDisk(u);
  auto r = glm::dot(d, d);
  auto h = std::sqrt(std::max((Float)0, 1 - r));
  return Vector3(d.x, h, d.y);
}
}  // namespace min::ray