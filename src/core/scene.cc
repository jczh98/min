
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
    accelerator = std::make_shared<Accel>();
    auto jsampler = json.at("sampler");
    auto jrenderer = json.at("renderer");
    auto jintegrator = json.at("integrator");
    auto jcamera = json.at("camera");
    auto jmeshes = json.at("meshes");
    sampler = CreateInstance<Sampler>(jsampler.at("type"), jsampler.at("props"));
    rendermode = CreateInstance<RenderMode>(jrenderer);
    integrator = CreateInstance<Integrator>(jintegrator);
    camera = CreateInstance<Camera>(jcamera.at("type"), jcamera.at("props"));
    for (auto jmesh : jmeshes) {
      auto mesh = CreateInstance<Mesh>(jmesh.at("type"), jmesh.at("props"));
      accelerator->AddMesh(mesh);
      meshes.emplace_back(mesh);
      if (mesh->IsLight()) {
        lights.emplace_back(mesh->GetLight());
      }
    }
    accelerator->Build();
  }
};
MIN_IMPLEMENTATION(Scene, SceneImpl, "default_scene")

}  // namespace min::ray
