
#include <min-ray/bitmap.h>
#include <min-ray/camera.h>
#include <min-ray/emitter.h>
#include <min-ray/integrator.h>
#include <min-ray/sampler.h>
#include <min-ray/scene.h>

namespace min::ray {

class SceneImpl : public Scene {
 public:
  void initialize(const json &json) override {
  }
};
MIN_IMPLEMENTATION(Scene, SceneImpl, "default_scene")

}  // namespace min::ray
