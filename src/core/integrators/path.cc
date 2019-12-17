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

#include "path.h"
#include <min-ray/parallel.h>
#include <min-ray/progress.h>
#include "../core/bsdfs/diffuse.h"

namespace min::ray {

Spectrum Li(const std::shared_ptr<Scene> &scene,
            const Ray &ray,
            const std::shared_ptr<Sampler> &sampler,
            Intersection *isect) {
}

void PathIntegrator::Render(const std::shared_ptr<Scene> &scene,
                            const std::shared_ptr<Camera> &camera,
                            const std::shared_ptr<Sampler> &sampler,
                            Film &film) {
  Point2i count(film.width, film.height);
  ProgressBar progress(count.x * count.y, 70);
  ParallelFor2D([&](Point2i p) {
    sampler->StartPixel({p.x, p.y}, Point2i(film.width, film.height));
    for (int s = 0; s < spp_; s++) {
      CameraSample camera_sample;
      camera->GenerateRay(sampler->Get2D(), sampler->Get2D(), {p.x, p.y}, Point2i(film.width, film.height), camera_sample);
      Intersection isect;
      if (scene->Intersect(camera_sample.ray, isect)) {
        film.AddSample(camera_sample.film, Li(scene, camera_sample.ray, sampler, &isect), 1.0 / spp_);
      }
    }
    ++progress;
    progress.Display();
  },
                count);
  progress.Done();
}
}  // namespace min::ray