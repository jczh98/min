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

#include <min-ray/film.h>

namespace min::ray {

static int toInt(float x) {
  return std::max<uint32_t>(0, std::min<uint32_t>(255, std::lroundf(gamma(x) * 255)));
}

void Film::WriteImage(const std::string &filename) {
  auto f = fopen(filename.c_str(), "w");
  fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
  for (int i = 0; i < width * height; i++)
    fprintf(f, "%d %d %d ",
            toInt(pixels[i].color[0] * pixels[i].weight),
            toInt(pixels[i].color[1] * pixels[i].weight),
            toInt(pixels[i].color[2] * pixels[i].weight));
}
}  // namespace min::ray