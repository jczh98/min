#include <min/math/linalg.h>
#include <min/visual/film.h>
#include <min/visual/camera.h>
#include <min/visual/renderer.h>
#include <min/visual/scene.h>
#include <fstream>

using namespace min;

int main() {
  try {
    std::string tmp = "E:\\work\\min-ray\\assets\\cornell_box\\test.json";
    std::ifstream is(tmp);
    Json j;
    is >> j;
    auto film = CreateInstance<Film>("film", j.at("film"));
    auto camera = CreateInstance<Camera>(j["camera"]["type"], GetProps(j.at("camera")));
    camera->SetFilm(film);
    auto scene = CreateInstance<Scene>("scene", "");
    scene->SetCamera(camera);
    auto renderer = CreateInstance<Renderer>(j["renderer"]["type"], GetProps(j.at("renderer")));
    renderer->SetScene(scene);
    renderer->Render();    
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}

