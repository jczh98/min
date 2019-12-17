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
#include "perspective.h"

namespace min::ray {

void PerspectiveCamera::GenerateRay(const Point2 &u1,
                                    const Point2 &u2,
                                    const Point2i &raster,
                                    Point2i dimension,
                                    CameraSample &sample) const {
  float x = Float(raster.x) / dimension.x;
  float y = 1 - Float(raster.y) / dimension.y;
  Point2 pixel_width(1.0 / dimension.x, 1.0 / dimension.y);
  sample.film = {x, y};
  sample.film += u1 * pixel_width - 0.5f * pixel_width;
  sample.lens = {0, 0};
  x = 2 * x - 1;
  y = 2 * y - 1;
  y *= float(dimension.y) / dimension.x;
  float z = 1 / std::atan(fov_.get() / 2);
  auto d = Vector3(x, y, 0) - Vector3(0, 0, -z);
  d = glm::normalize(d);
  auto o = Vector3(sample.lens.x, sample.lens.y, 0);
  o = CameraToWorld(o) + viewpoint_;
  d = CameraToWorld(d);
  sample.ray = Ray(o, d, RayBias);
}
}  // namespace min::ray