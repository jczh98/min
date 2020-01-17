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
  Float x = Float(raster.x / dimension.x);
  Float y = Float(raster.y / dimension.y);

  Point2 pixelWidth(1.0 / dimension.x, 1.0 / dimension.y);
  sample.film = raster;
  auto p = Point2(x, y) + u1 * pixelWidth - 0.5f * pixelWidth;
  sample.lens = {0, 0};
  x = p[0];
  y = p[1];
  y = 1 - y;
  x = -(2 * x - 1);
  y = 2 * y - 1;
  y *= Float(dimension.y / dimension.x);
  Float z = 1.0f / std::atan(fov.get() / 2);
  Vector3 d = Vector3(x, y, 0) - Vector3(0, 0, -z);
  d = normalize(d);
  Point3 o = Vector3(sample.lens.x, sample.lens.y, 0);
  o = transform.TransformPoint3(o);
  d = transform.TransformVector3(d);
  sample.ray = Ray(o, d, RayBias);
}

void PerspectiveCamera::Preprocess() {
  transform = manipulator.ToTransform();
  inv_transform = transform.inverse();
}
MIN_IMPLEMENTATION(Camera, PerspectiveCamera, "perspective")
}  // namespace min::ray