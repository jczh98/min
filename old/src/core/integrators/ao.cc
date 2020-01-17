#include "ao.h"
#include <min-ray/film.h>
#include <min-ray/parallel.h>
#include <min-ray/sampling.h>
#include <min-ray/progress.h>

namespace min::ray {

void AOIntegrator::Render(const std::shared_ptr<Scene> &scene,
                          const std::shared_ptr<Camera> &camera,
                          const std::shared_ptr<Sampler> &sampler,
                          Film &film) {
  Point2i count(film.width, film.height);
  ProgressBar progress(film.width * film.height, 70);
  ParallelFor2D([&](Point2i p) {
    int i = p.x, j = p.y;
    for (int s = 0; s < spp_; s++) {
      CameraSample camera_sample;
      sampler->StartNextSample();
      camera->GenerateRay(sampler->Get2D(), sampler->Get2D(), Point2i(i, j), Point2i(film.width, film.height), camera_sample);
      Intersection isect;
      if (scene->Intersect(camera_sample.ray, isect)) {
        MIN_DEBUG("fuck");
        isect.ComputeLocalFrame();
        auto wo = isect.WorldToLocal(isect.wo);
        auto w = CosineHemisphereSampling(sampler->Get2D());
        if (wo.y * w.y < 0) {
          w = -w;
        }
        w = isect.LocalToWorld(w);
        auto ray = isect.SpawnRay(w);
        isect = Intersection();
        if (!scene->Intersect(ray, isect) || isect.distance >= occlude_distance_) {
          film.AddSample(camera_sample.film, Spectrum(1), 1.0 / spp_);
        }
      }
    }
    ++progress;
    progress.Display();
  },
                count);
  progress.Done();
}
}  // namespace min::ray