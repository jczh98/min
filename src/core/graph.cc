#include <min-ray/film.h>
#include <min-ray/graph.h>

namespace min::ray {

void SceneGraph::Render() {
  Film film(file_dimension);
  auto scene = std::make_shared<Scene>();
  integrator_->Render(scene, camera_, sampler_, film);
}

}  // namespace min::ray