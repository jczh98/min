#include <min-ray/film.h>
#include <min-ray/parallel.h>
#include <iostream>
#include "../core/accelerators/sahbvh.h"
#include "../core/bsdfs/diffuse.h"
#include "../core/cameras/perspective.h"
#include "../core/integrators/ao.h"
#include "../core/lights/diffuse_area.h"
#include "../core/samplers/random.h"
#include "../core/shaders/common.h"
#include "../core/shapes/sphere.h"

using namespace min::ray;

int main(int, char**) {
  ParallelInit(4);
  std::shared_ptr<Camera> camera(new PerspectiveCamera(Vector3(50.0, 40.8, 220.0), Vector3(50.0, 40.8, 0.0), DegreesToRadians<Float>(60)));
  Film film{1080 / 2, 720 / 2};
  std::shared_ptr<Scene> scene = std::make_shared<Scene>();
  scene->primitives() = {
      std::make_shared<Sphere>(6.0, Vector3(10, 70, 51.6), nullptr, std::make_shared<DiffuseAreaLight>(Vector3(100., 100., 100.))),
      std::make_shared<Sphere>(1e5, Vector3(1e5 + 1, 40.8, 81.6),
                               std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(0.7f * Vector3(0.75, 0.25, 0.25)))),
      std::make_shared<Sphere>(1e5, Vector3(-1e5 + 99, 40.8, 81.6),
                               std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(0.7f * Vector3(0.25, 0.25, 0.75)))),
      std::make_shared<Sphere>(1e5, Vector3(50, 40.8, 1e5),
                               std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(0.7f * Vector3(0.75, 0.65, 0.75)))),
      std::make_shared<Sphere>(1e5, Vector3(50, 40.8, -1e5 + 350),
                               std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(0.7f * Vector3(0.50, 0.50, 0.50)))),
      std::make_shared<Sphere>(1e5, Vector3(50, 1e5, 81.6),
                               std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(0.7f * Vector3(0.65, 0.75, 0.75)))),
      std::make_shared<Sphere>(1e5, Vector3(50, -1e5 + 81.6, 81.6),
                               std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(0.7f * Vector3(0.75, 0.75, 0.65)))),
      std::make_shared<Sphere>(20, Vector3(50, 20, 50),
                               std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(Vector3(0.25, 0.75, 0.25)))),
      std::make_shared<Sphere>(16.5, Vector3(19, 16.5, 25),
                               std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(Vector3(0.99, 0.99, 0.99)))),
      std::make_shared<Sphere>(16.5, Vector3(77, 16.5, 78),
                               std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(Vector3(0.99, 0.99, 0.99))))};
  scene->Preprocess();
  scene->accelerator() = std::make_shared<BVHAccelerator>(scene->primitives());
  std::shared_ptr<Sampler> sampler(new RandomSampler());
  std::shared_ptr<Integrator> integrator(new AOIntegrator(64));
  integrator->Render(scene, camera, sampler, film);
  film.WriteImage("out.ppm");
  ParallelCleanUp();
  return 0;
}
