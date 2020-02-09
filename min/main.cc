#include <min/math/linalg.h>
#include <min/visual/film.h>

using namespace min;

int main() {
  auto film = CreateInstance<Film>("film", "");
  auto sample_bounds = film->GetSampleBounds();
  auto sample_extent = sample_bounds.Diagonal();
  const int kTileSize = 16;
  Point2i tiles = Point2i((sample_extent.x + kTileSize - 1) / kTileSize, (sample_extent.y + kTileSize - 1) / kTileSize);
  for (int y = 0; y < tiles.y; y++) {
    for (int x = 0; x < tiles.x; x++) {
      Point2i tile(x, y);
      int x0 = sample_bounds.pmin.x + tile.x * kTileSize;
      int x1 = std::min(x0 + kTileSize, sample_bounds.pmax.x);
      int y0 = sample_bounds.pmin.y + tile.y * kTileSize;
      int y1 = std::min(y0 + kTileSize, sample_bounds.pmax.y);
      Bounds2i tile_bounds(Point2i(x0, y0), Point2i(x1, y1));
      MIN_INFO("Starting image tile {}", tile_bounds.ToString());
      auto film_tile = film->GetFilmTile(tile_bounds);
      for (Point2i pixel : tile_bounds) {
      }
    }
  }
  return 0;
}

