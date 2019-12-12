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

#include "defs.h"
#include "math.h"

namespace min::ray {
class Film {
 public:
  struct Pixels {
    explicit Pixels(int n) : color(n) {}
    std::vector<Vector4> color;
    std::vector<Float> weight;
  };
  struct Pixel {
    Pixel(Vector4 color, Float weight) : color(color), weight(weight) {}
    Vector4 color;
    Float weight;
  };

  explicit Film(const Vector2i &dim) : Film(dim.x(), dim.y()) {
  }

  Film(size_t w, size_t h) : width(w), height(h), pixels(w * h) {}

  Pixel operator()(const Vector2 &p) { return (*this)(p.x, p.y); }

  Pixel operator()(Float x, Float y) {
    int px = std::clamp<int>(std::lround(x * width), 0, width - 1);
    int py = std::clamp<int>(std::lround(y * height), 0, height - 1);
    return Pixel(pixels.color.at(px + py * width), pixels.weight.at(px + py * width));
  }

  Pixel operator()(int px, int py) { return Pixel(pixels.color.at(px + py * width), pixels.weight.at(px + py * width)); }

  void AddSample(const Vector2 &p, const Vector3 &color, Float weight) {
    auto pixel = (*this)(p);
    pixel.color += Vector4(color * weight, 0);
    pixel.weight += weight;
  }

  void WriteImage(const std::string &filename);

  Pixels pixels;
  const size_t width, height;
};
}  // namespace min::ray