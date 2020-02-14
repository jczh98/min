#include <min/visual/camera.h>

namespace min {

class PerspectiveCamera : public Camera {
  Transform camera2screen, raster2camera;
  Transform screen2raster, raster2screen;
  Float fov;
 public:
  void initialize(const Json &json) override {
    fov = Value(json, "fov", 90.f);
    film = CreateInstance<Film>("film", json.at("film"));
    camera2world = Value(json, "transform", Transform());
    camera2screen = Perspective(fov, 1e-2f, 1000.0f);
    Float aspect_ration = Float(film->full_resolution.x) / Float(film->full_resolution.y);
    Bounds2f screen_window;
    if (aspect_ration > 1.0f) {
      screen_window = Bounds2f(Vector2f(-aspect_ration, -1.0f), Vector2f(aspect_ration, 1.0f));
    } else {
      screen_window = Bounds2f(Vector2f(-1.0f, -1.0f / aspect_ration), Vector2f(1.0f, 1.0f / aspect_ration));
    }
    screen2raster = Scale(film->full_resolution.x, film->full_resolution.y, 1) *
        Scale(1.0f / (screen_window.pmax.x - screen_window.pmin.x),
              1.0f / (screen_window.pmin.y - screen_window.pmax.y), 1) *
        Translate(Vector3f(-screen_window.pmin.x, -screen_window.pmax.y, 0));
    raster2screen = Inverse(screen2raster);
    raster2camera = Inverse(camera2screen) * raster2screen;
  }
  Float GenerateRay(const Point2f &pfilm, const Point2f &plens, Float time, Ray &ray) const override {
    Point3f pcamera = raster2camera.ToPoint(Point3f(pfilm.x, pfilm.y, 0));
    ray = Ray(Point3f(0, 0, 0), Normalize(pcamera));
    ray.time = time;
    ray.o = camera2world.ToPoint(ray.o);
    ray.d = camera2world.ToVector(ray.d);
    return 1;
  }
};
MIN_IMPLEMENTATION(Camera, PerspectiveCamera, "perspective")

}

