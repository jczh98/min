#include <min-ray/film.h>
#include <min-ray/parallel.h>
#include <min-ray/scene.h>
#include <min-ray/integrator.h>
#include <min-ray/sampler.h>
#include <iostream>
#include "../core/integrators/ao.h"
#include "../core/cameras/perspective.h"
#include "../core/importers/wavefront.h"
#include "../core/samplers/random.h"
#include "../core/accelerators/brute.h"

using namespace min::ray;

int main(int, char **) {
  ParallelInit(4);
  auto camera = std::make_shared<PerspectiveCamera>();
  auto jcamera = R"({ 
    "fov": {
        "rad": 0.5235987901687622
    },
    "transform" : {
      "rotation" : {
        "rad" : [
          3.1415927410125734,
          0.0,
          0.0
        ]
      },
        "translation" : [
          0,
          1,
          5
        ]
    }
})"_json;
  camera->initialize(jcamera);

  auto importer = std::make_shared<WavefrontImporter>();
  auto ret = importer->Import("assets/cornell_box/CornellBox-Original.obj");
  Film film{1080, 720};
  auto scene = std::make_shared<Scene>();
  
  auto integrator = std::make_shared<AOIntegrator>(1);
  std::shared_ptr<Sampler> sampler(new RandomSampler());
  
  camera->Preprocess();
  
  scene->accelerator = std::make_shared<Bruteforce>();
  scene->meshes.emplace_back(ret.mesh);
 
  scene->Preprocess();
  integrator->Render(scene, camera, sampler, film);
  film.WriteImage("out.ppm");
  //std::shared_ptr<Camera> camera(new PerspectiveCamera(Vector3(50.0, 40.8, 220.0), Vector3(50.0, 40.8, 0), DegreesToRadians<Float>(60)));
  // std::shared_ptr<Camera> camera(new PerspectiveCamera(Vector3(0, 0, 7), Vector3(0, 0, 0), DegreesToRadians<Float>(16)));
  // Film film{1080, 720};

  // std::shared_ptr<Scene> scene = std::make_shared<Scene>();

  // auto importer = WavefrontImporter();
  // auto obj = importer.Import(fs::path("assets/bunny/bunny.obj"));
  // auto convert = [&](MeshImportResult result) -> std::vector<std::shared_ptr<GeometricPrimitive>> {
  //   std::vector<std::shared_ptr<GeometricPrimitive>> ret;
  //   for (auto &x : result.triangles) {
  //     ret.emplace_back(std::make_shared<GeometricPrimitive>(std::move(x),
  //                                                           nullptr,
  //                                                           std::make_shared<DiffuseAreaLight>(Vector3(100., 100., 100.))));
  //   }
  //   return ret;
  // };
  // auto objs = convert(obj);
  // auto sphere1 = std::make_shared<GeometricPrimitive>(std::make_shared<Sphere>(6.0, Vector3(10, 70, 51.6)),
  //                                                     nullptr,
  //                                                     std::make_shared<DiffuseAreaLight>(Vector3(100., 100., 100.)));
  // auto mesh = std::make_shared<MeshTriangle>();
  // mesh->positions.emplace_back(Vector3(-2, 0, 0));
  // mesh->positions.emplace_back(Vector3(0, 2, 0));
  // mesh->positions.emplace_back(Vector3(1, 0, 0));
  // mesh->vertex_indices.emplace_back(0);
  // mesh->vertex_indices.emplace_back(1);
  // mesh->vertex_indices.emplace_back(2);
  // mesh->face_indices.emplace_back(0);
  // auto tri = std::make_shared<GeometricPrimitive>(std::make_shared<Triangle>(mesh, 0), std::make_shared<Matte>(std::make_shared<ConstantTexture>(Vector3(0.6300, 0.0650, 0.0500))));
  // scene->primitives()
  //     .insert(scene->primitives().end(), objs.begin(), objs.end());
  // scene->primitives().insert(scene->primitives().end(), sphere1);
  // //scene->primitives().insert(scene->primitives().end(), tri);

  // // auto sphere1 = std::make_shared<GeometricPrimitive>(std::make_shared<Sphere>(6.0, Vector3(10, 70, 51.6)), nullptr, std::make_shared<DiffuseAreaLight>(Vector3(100., 100., 100.)));
  // // auto sphere2 = std::make_shared<GeometricPrimitive>(std::make_shared<Sphere>(1e5, Vector3(1e5 + 1, 40.8, 81.6)),
  // //                                                     std::make_shared<Matte>(std::make_shared<ConstantTexture>(0.7f * Vector3(0.75, 0.25, 0.25))));
  // // auto sphere3 = std::make_shared<GeometricPrimitive>(std::make_shared<Sphere>(1e5, Vector3(-1e5 + 99, 40.8, 81.6)),
  // //                                                     std::make_shared<Matte>(std::make_shared<ConstantTexture>(0.7f * Vector3(0.25, 0.25, 0.75))));
  // // auto sphere4 = std::make_shared<GeometricPrimitive>(std::make_shared<Sphere>(1e5, Vector3(50, 40.8, 1e5)),
  // //                                                     std::make_shared<Matte>(std::make_shared<ConstantTexture>(0.7f * Vector3(0.75, 0.65, 0.75))));
  // // auto sphere5 = std::make_shared<GeometricPrimitive>(std::make_shared<Sphere>(1e5, Vector3(50, 40.8, -1e5 + 350)),
  // //                                                     std::make_shared<Matte>(std::make_shared<ConstantTexture>(0.7f * Vector3(0.50, 0.50, 0.50))));
  // // auto sphere6 = std::make_shared<GeometricPrimitive>(std::make_shared<Sphere>(1e5, Vector3(50, 1e5, 81.6)),
  // //                                                     std::make_shared<Matte>(std::make_shared<ConstantTexture>(0.7f * Vector3(0.65, 0.75, 0.75))));
  // // auto sphere7 = std::make_shared<GeometricPrimitive>(std::make_shared<Sphere>(1e5, Vector3(50, -1e5 + 81.6, 81.6)),
  // //                                                     std::make_shared<Matte>(std::make_shared<ConstantTexture>(0.7f * Vector3(0.75, 0.75, 0.65))));
  // // auto sphere8 = std::make_shared<GeometricPrimitive>(std::make_shared<Sphere>(20, Vector3(50, 20, 50)),
  // //                                                     std::make_shared<Matte>(std::make_shared<ConstantTexture>(Vector3(0.25, 0.75, 0.25)), Radians<Float>(Degrees<Float>(20))));
  // // auto sphere9 = std::make_shared<GeometricPrimitive>(std::make_shared<Sphere>(16.5, Vector3(19, 16.5, 25)),
  // //                                                     std::make_shared<Matte>(std::make_shared<ConstantTexture>(Vector3(0.99, 0.99, 0.99))));
  // // auto sphere10 = std::make_shared<GeometricPrimitive>(std::make_shared<Sphere>(16.5, Vector3(77, 16.5, 78)),
  // //                                                      std::make_shared<Matte>(std::make_shared<ConstantTexture>(Vector3(0.99, 0.99, 0.99))));
  // // scene->primitives() = {
  // //     sphere1,
  // //     sphere2,
  // //     sphere3,
  // //     sphere4,
  // //     sphere5,
  // //     sphere6,
  // //     sphere7,
  // //     sphere8,
  // //     sphere9,
  // //     sphere10,
  // // };

  // //scene->primitives().insert(scene->primitives().end(), tri);

  // scene->Preprocess();
  // scene->accelerator() = std::make_shared<BVHAccelerator>(scene->primitives());
  // std::shared_ptr<Sampler> sampler(new RandomSampler());
  // std::shared_ptr<Integrator> integrator(new PathIntegrator(1));
  // integrator->Render(scene, camera, sampler, film);
  // film.WriteImage("out.ppm");
  ParallelCleanUp();
  return 0;
}
