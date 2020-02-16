#include <min/visual/renderer.h>
#include <min/visual/scene.h>
#include <min/visual/film.h>
#include <min/visual/sampler.h>
#include <min/common/json.h>
#include <min/gui/preview_gui.h>
#include <min/common/parallel.h>

namespace min {

class PathTracer : public Renderer {
  std::shared_ptr<Sampler> sampler;
 public:
  void initialize(const Json &json) override {
    sampler = CreateInstance<Sampler>(json["sampler"]["type"],GetProps(json.at("sampler")));
  }
  void Render() override {
    auto camera = scene->camera;
    auto film = camera->film;
    auto sample_bounds = film->GetSampleBounds();
    auto sample_extent = sample_bounds.Diagonal();
    auto gui = std::make_unique<PreviewGUI>(film);
    gui->Init();
    const int kTileSize = 16;
    Point2i tiles = Point2i((sample_extent.x + kTileSize - 1) / kTileSize, (sample_extent.y + kTileSize - 1) / kTileSize);
    std::thread render_thread([&] {
      MIN_INFO("Rendering .. ");
      ParallelFor2D([&](Vector2i tile) {
        auto tile_sampler = sampler->Clone();
        int x0 = sample_bounds.pmin.x + tile.x * kTileSize;
        int x1 = std::min(x0 + kTileSize, sample_bounds.pmax.x);
        int y0 = sample_bounds.pmin.y + tile.y * kTileSize;
        int y1 = std::min(y0 + kTileSize, sample_bounds.pmax.y);
        Bounds2i tile_bounds(Point2i(x0, y0), Point2i(x1, y1));
        //MIN_INFO("Starting image tile {}", tile_bounds.ToString());
        auto film_tile = film->GetFilmTile(tile_bounds);
        for (Point2i pixel : tile_bounds) {
          for (int s = 0; s < tile_sampler->spp; s++) {
            tile_sampler->StartPixel(pixel);
            Ray ray;
            auto pfilm = (Point2f)pixel + sampler->Get2D();
            auto ray_weight = camera->GenerateRay(pfilm,sampler->Get2D(), sampler->Get1D(), ray);
            //MIN_DEBUG("o : {} d : {}", ray.o.ToString(), ray.d.ToString());
            Spectrum L(0.f);
            if (ray_weight > 0) L = Li(ray, scene, *tile_sampler);
            if (L.Abnormal()) {
              MIN_WARN("Not a number radiance value returned for pixel ({}, {}), sample {}. Setting to black.",
                       pixel.x, pixel.y, s);
              L = Spectrum(0.f);
            } else if (L.y < -1e-5) {
              MIN_WARN("Negative luminance value, {}, returned for pixel ({}, {}), sample {}. Setting to black.",
                       L.y, pixel.x, pixel.y, s);
              L = Spectrum(0.f);
            } else if (std::isinf(L.y)) {
              MIN_WARN("Infinite luminance value returne returned for pixel ({}, {}), sample {}. Setting to black.",
                       pixel.x, pixel.y, s);
              L = Spectrum(0.f);
            }
            film_tile->AddSample(pfilm, L, ray_weight);
          }
        }
        film->MergeFilmTile(std::move(film_tile));
      }, tiles);
      MIN_INFO("Done.");
    });
    gui->Mainloop();
    render_thread.join();
    gui->Shutdown();
    film->WriteImage();
  }

  Spectrum Li(const Ray &ray, const std::shared_ptr<Scene> &scene, Sampler &sampler) {
    SurfaceIntersection isect;
    if (!scene->Intersect(ray, isect)) {
      return Spectrum(0);
    }
    return Spectrum(Abs(isect.shading_frame.n));
  }
};
MIN_IMPLEMENTATION(Renderer, PathTracer, "pt")

}

