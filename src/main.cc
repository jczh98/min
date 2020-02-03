#include <min-ray/scene.h>
#include <min-ray/camera.h>
#include <min-ray/block.h>
#include <min-ray/timer.h>
#include <min-ray/bitmap.h>
#include <min-ray/sampler.h>
#include <min-ray/integrator.h>
#include <min-ray/scene.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <min-ray/resolver.h>
#include <thread>
#include <fstream>
#include <filesystem>

using namespace min::ray;

int main(int argc, char **argv) {
    std::string tmp = "E:\\work\\min-ray\\assets\\bunny\\scene.json";
    fs::path path(tmp);
    GetFileResolver()->Prepend(path.parent_path());
    std::ifstream is(tmp);
    json j;
    is >> j;
    auto jsampler = j.at("sampler");
    auto jrenderer = j.at("renderer");
    auto jcamera = j.at("camera");
    auto jmeshes = j.at("meshes");
    auto accelerator = CreateInstance<Accelerator>(j["accelerator"]["type"]);
    auto scene = CreateInstance<Scene>("default_scene");
    auto sampler = CreateInstance<Sampler>(jsampler.at("type"), jsampler.at("props"));
    auto camera = CreateInstance<Camera>(jcamera.at("type"), jcamera.at("props"));
    auto renderer = CreateInstance<Renderer>(jrenderer.at("type"), jrenderer.at("props"));
    scene->accelerator = accelerator;
    for (auto jmesh : jmeshes) {
      auto mesh = CreateInstance<Mesh>(jmesh.at("type"), jmesh.at("props"));
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
