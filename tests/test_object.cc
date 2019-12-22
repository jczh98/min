#include <min-ray/context.h>
#include <min-ray/json.h>
#include <min-ray/object.h>
#include <iostream>

struct Film : public min::refl::Object {
  int width, height;
  void Initialize(const nlohmann::json& json) override {
    width = json["width"];
    height = json["height"];
  }
};
class Scene : public min::refl::Object {
 public:
  Film *film;
  int x, y;
  void Initialize(const nlohmann::json& json) override {
    x = json["x"];
    y = json["y"];
    film = min::refl::ref<Film>(json, "film");
  }
};
MIN_IMPL(Film, "film::default")
MIN_IMPL(Scene, "scene::default")
int main() {
  min::refl::Context::instance().Reset();
  auto film = min::refl::Load<Film>("film", "film::default", {{"width", 120}, {"height", 240}});
  auto scene = min::refl::Load<Scene>("scene", "scene::default", {{"x", 33}, {"y", 44}, {"film", film->loc()}});
  std::cout << scene->film->width << std::endl;
  return 0;
}