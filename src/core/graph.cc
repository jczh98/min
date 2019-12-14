#include <min-ray/film.h>
#include <min-ray/graph.h>
#include "cameras/perspective.h"
#include "integrators/ao.h"
#include "samplers/random.h"

namespace min::ray {

void SceneGraph::Render() {
  file_dimension = {1280, 720};
  Film film(file_dimension);
  camera_ = std::make_shared<PerspectiveCamera>(Vector3(0.0, 2.0, 9.0), Vector3(180, 0, 0));
  sampler_ = std::make_shared<RandomSampler>();
  integrator_ = std::make_shared<AOIntegrator>(4, 4);
  auto scene = std::make_shared<Scene>();
  integrator_->Render(scene, camera_, sampler_, film);
}

}  // namespace min::ray