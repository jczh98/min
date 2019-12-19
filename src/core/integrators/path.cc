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
#include <min-ray/shape.h>

namespace min::ray {

static Float MISWeight(Float p1, Float p2) {
  p1 *= p1;
  p2 *= p2;
  return p1 / (p1 + p2);
}

Spectrum PathIntegrator::Li(const std::shared_ptr<Scene> &scene, const Ray &ray,
                            const std::shared_ptr<Sampler> &sampler,
                            Intersection *isect) {
  Spectrum L(0);
  Spectrum beta(1);
  const int max_depth = 4;
  Intersection nisect;
  if (isect) {
    nisect = *isect;
  }
  int depth = 0;
  Ray nray = ray;
  Intersection prevIsct;
  BSDFSample sample;
  bool is_specular = false;
  while (true) {
    if (nisect.shape == nullptr) break;
    nisect.ComputeLocalFrame();
    auto light = nisect.primitive->GetAreaLight();
    ShadingPoint sp;
    sp.tex_coords = nisect.uv;
    sp.ng = nisect.ng;
    sp.ns = nisect.ns;
    if (light) {
      if (depth == 0 || is_specular) {
        L += beta * light->Li(sp);
      } else {
        Float scattering_pdf = sample.pdf;
        Float light_pdf = 1.0f / scene->lights().size() * light->PdfLi(prevIsct, nray.d);
        auto w = MISWeight(scattering_pdf, light_pdf);
        L += beta * light->Li(sp) * w;
      }
    }
    if (++depth > max_depth) {
      break;
    }
    nisect.ComputeScatteringFunctions();
    if (!nisect.bsdf) {
      break;
    }
    sample = BSDFSample();
    sample.wo = -glm::normalize(nisect.WorldToLocal(nray.d));
    nisect.bsdf->Sample(sampler->Get2D(), sp, sample);
    if (sample.pdf <= 0) break;

    is_specular = sample.sample_type & BSDF::ESpecular;

    auto sampled_light = scene->SampleLight(sampler);
    if (sampled_light) {
      LightSample lightSample;
      VisibilityTester visibilityTester;
      sampled_light->SampleLi(sampler->Get2D(), nisect, lightSample, visibilityTester);
      auto wi = nisect.WorldToLocal(lightSample.wi);
      auto f = nisect.bsdf->Evaluate(sp, sample.wo, wi) * glm::abs(glm::dot(lightSample.wi, nisect.ns));
      Float light_pdf = lightSample.pdf / scene->lights().size();
      Float scattering_pdf = nisect.bsdf->EvaluatePdf(sp, sample.wo, wi);
      if (!IsBlack(f) && visibilityTester.Visible(scene)) {
        if (is_specular) {
          L += beta * f * lightSample.li / light_pdf;
        } else {
          auto w = MISWeight(light_pdf, scattering_pdf);
          L += beta * f * lightSample.li / light_pdf * w;
        }
      }
    }

    auto wi = nisect.LocalToWorld(sample.wi);
    beta *= sample.s / sample.pdf * std::abs(glm::dot(wi, nisect.ng));
    nray = nisect.SpawnRay(wi);
    prevIsct = nisect;
    nisect = Intersection();
    scene->Intersect(nray, nisect);
  }
  return L;
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