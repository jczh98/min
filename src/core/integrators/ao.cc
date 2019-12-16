#include "ao.h"
#include <min-ray/film.h>
#include <min-ray/sampling.h>

namespace min::ray {

void AOIntegrator::Render(const std::shared_ptr<Scene> &scene,
                          const std::shared_ptr<Camera> &camera,
                          const std::shared_ptr<Sampler> &sampler,
                          Film &film) {
  for (int j = 0; j < film.height; j++) {
    for (int i = 0; i < film.width; i++) {
      fmt::print("{} {}\n", i, j);
      sampler->StartPixel(Point2i(i, j), Point2i(film.width, film.height));
      for (int s = 0; s < spp_; s++) {
        CameraSample camera_sample;
        sampler->StartNextSample();
        camera->GenerateRay(sampler->Get2D(), sampler->Get2D(), Point2i(i, j), Point2i(film.width, film.height), camera_sample);
        Intersection isect;
        if (scene->Intersect(camera_sample.ray, isect)) {
          isect.ComputeLocalFrame();
          auto wo = isect.WorldToLocal(-camera_sample.ray.d);
          auto w = CosineHeisphereSampling(sampler->Get2D());
          if (wo.y * w.y < 0) {
            w = -w;
          }
          auto ray = isect.SpawnRay(w);
          isect = Intersection();
          if (!scene->Intersect(ray, isect) || isect.distance >= occlude_distance_) {
            //film.AddSample(camera_sample.film, Spectrum(1), 1.0 / spp_);
            film(camera_sample.film) += Spectrum(1);
          } else {
            //film.AddSample(camera_sample.film, Spectrum(0), 0);
          }
        }
      }
    }
  }
  film.weight = 1.0f / spp_;
}
}  // namespace min::ray