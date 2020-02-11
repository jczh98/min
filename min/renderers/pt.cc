#include <min/visual/renderer.h>
#include <min/visual/scene.h>
#include <min/visual/film.h>
#include <min/visual/sampler.h>
#include <min/common/json.h>

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
    const int kTileSize = 16;
    Point2i tiles = Point2i((sample_extent.x + kTileSize - 1) / kTileSize, (sample_extent.y + kTileSize - 1) / kTileSize);
    for (int y = 0; y < tiles.y; y++) {
      for (int x = 0; x < tiles.x; x++) {
        Point2i tile(x, y);
        auto tile_sampler = sampler->Clone();
        int x0 = sample_bounds.pmin.x + tile.x * kTileSize;
        int x1 = std::min(x0 + kTileSize, sample_bounds.pmax.x);
        int y0 = sample_bounds.pmin.y + tile.y * kTileSize;
        int y1 = std::min(y0 + kTileSize, sample_bounds.pmax.y);
        Bounds2i tile_bounds(Point2i(x0, y0), Point2i(x1, y1));
        MIN_INFO("Starting image tile {}", tile_bounds.ToString());
        auto film_tile = film->GetFilmTile(tile_bounds);
        for (Point2i pixel : tile_bounds) {
          tile_sampler->StartPixel(pixel);
        }
      }
    }
  }
};
MIN_IMPLEMENTATION(Renderer, PathTracer, "pt")

}

