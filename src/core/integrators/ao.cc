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
      sampler->StartPixel(Point2i(i, j), Point2i(film.width, film.height));
      for (int s = 0; s < spp_; s++) {
        CameraSample camera_sample;
        sampler->StartNextSample();
        camera->GenerateRay(sampler->Get2D(), sampler->Get2D(), Point2i(i, j), Point2i(film.width, film.height), camera_sample);
        Intersection isect;
        if (scene->Intersect(camera_sample.ray, isect)) {
          auto wo = isect.WorldToLocal(-camera_sample.ray.d);
          auto w = CosineHeisphereSampling(sampler->Get2D());
          if (wo.y * w.y < 0) {
            w = -w;
          }
          w = isect.LocalToWorld(w);
          auto ray = isect.SpawnRay(w);
          ray.tmax = occlude_distance_;
          isect = Intersection();
          if (!scene->Intersect(ray, isect) || isect.distance >= occlude_distance_) {
            film.AddSample(camera_sample.pfilm, Spectrum(1), 1);
          } else {
            film.AddSample(camera_sample.pfilm, Spectrum(0), 0);
          }
        }
      }
    }
  }
}
}  // namespace min::ray