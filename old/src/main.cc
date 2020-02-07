#include <min-ray/scene.h>
#include <min-ray/camera.h>
#include <min-ray/block.h>
#include <min-ray/timer.h>
#include <min-ray/bitmap.h>
#include <min-ray/sampler.h>
#include <min-ray/json.h>
#include <min-ray/integrator.h>
#include <min-ray/scene.h>
#include <min-ray/bsdf.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <min-ray/resolver.h>
#include <thread>
#include <fstream>
#include <filesystem>

using namespace min::ray;

int main(int argc, char **argv) {
    std::string tmp = "E:\\work\\min-ray\\assets\\cornell_box\\scene.json";
    fs::path path(tmp);
    GetFileResolver()->Prepend(path.parent_path());
    std::ifstream is(tmp);
    Json j;
    is >> j;
    auto jsampler = j.at("sampler");
    auto jrenderer = j.at("renderer");
    auto jcamera = j.at("camera");
    auto jmeshes = j.at("meshes");
    auto accelerator = CreateInstance<Accelerator>(j["accelerator"]["type"], rjson::GetProps(j["accelerator"]));
    auto scene = CreateInstance<Scene>("default_scene");
    auto sampler = CreateInstance<Sampler>(jsampler.at("type"), rjson::GetProps(jsampler));
    auto camera = CreateInstance<Camera>(jcamera.at("type"), rjson::GetProps(jcamera));
    auto renderer = CreateInstance<Renderer>(jrenderer.at("type"), rjson::GetProps(jrenderer));
    scene->accelerator = accelerator;
    for (auto jmesh : jmeshes) {
      auto mesh = CreateInstance<Mesh>(jmesh.at("type"), rjson::GetProps(jmesh));
      scene->AddPrimitive(mesh);
    }
    scene->Build();
    scene->sampler = sampler;
    scene->camera = camera;
    renderer->scene = scene;
    renderer->filename = tmp;
    renderer->Render();
    return 0;
}
