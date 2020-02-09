#pragma once

#include "defs.h"
#include "filter.h"
#include "spectrum.h"
#include "geometry.h"
#include <mutex>

namespace min {

struct FilmTilePixel {
  Spectrum contrib_sum = Spectrum(0.f);
  Float filter_weight_sum = 0.0f;
};

class Film : public Unit {
  struct Pixel {
    Pixel() { value = Spectrum(0.f); filter_weight_sum = 0; }
    Spectrum value;
    Float filter_weight_sum;
  };
  std::unique_ptr<Pixel[]> pixels;
  static constexpr int filter_table_width = 16;
  Float filter_table[filter_table_width * filter_table_width];
  std::mutex mutex;
  Float scale;
  Float max_sample_luminance;

  // Film Private Methods
  Pixel &GetPixel(const Point2i &p) {
    int width = cropped_pixel_bounds.pmax.x - cropped_pixel_bounds.pmin.x;
    int offset = (p.x - cropped_pixel_bounds.pmin.x) +
        (p.y - cropped_pixel_bounds.pmin.y) * width;
    return pixels[offset];
  }
 public:
  Point2i full_resolution;
  std::unique_ptr<Filter> filter;
  Float diagonal;
  std::string filename;
  Bounds2i cropped_pixel_bounds;

  void initialize(const Json &json) override {

  }

  void Initialize(const Point2i &resolution, const Bounds2f &crop_window,
                  std::unique_ptr<Filter> filt, Float diagona,
                  const std::string &filenam, Float scal, Float max_sample_luminanc) {
    full_resolution = resolution;
    diagonal = diagona * .001;
    filter = std::move(filt);
    filename = filenam;
    scale = scal;
    max_sample_luminance = max_sample_luminanc;
    cropped_pixel_bounds =
        Bounds2i(Point2i(std::ceil(full_resolution.x * crop_window.pmin.x),
                         std::ceil(full_resolution.y * crop_window.pmin.y)),
                 Point2i(std::ceil(full_resolution.x * crop_window.pmax.x),
                         std::ceil(full_resolution.y * crop_window.pmax.y)));
    MIN_INFO("Created film with full resolution {}. Crop window of {} -> croppedPixelBounds {}", crop_window.ToString(), cropped_pixel_bounds.ToString());

    // Allocate film image storage
    pixels = std::unique_ptr<Pixel[]>(new Pixel[cropped_pixel_bounds.Area()]);

    // Precompute filter weight table
    int offset = 0;
    for (int y = 0; y < filter_table_width; ++y) {
      for (int x = 0; x < filter_table_width; ++x, ++offset) {
        Point2f p;
        p.x = (x + 0.5f) * filter->radius.x / filter_table_width;
        p.y = (y + 0.5f) * filter->radius.y / filter_table_width;
        filter_table[offset] = filter->Evaluate(p);
      }
    }
  }
  Bounds2i GetSampleBounds() const;
  Bounds2f GetPhysicalExtent() const;
  std::unique_ptr<FilmTile> GetFilmTile(const Bounds2i &sample_bounds);
  void MergeFilmTile(std::unique_ptr<FilmTile> tile);
  void SetImage(Spectrum *img) const;
  void WriteImage(Float splatScale = 1);
  void Clear();
};

class FilmTile {
  const Bounds2i pixel_bounds;
  const Vector2f filter_radius, inv_filter_radius;
  const Float *filter_table;
  const int filter_table_size;
  std::vector<FilmTilePixel> pixels;
  const Float max_sample_luminance;
  friend class Film;
 public:
  FilmTile(const Bounds2i &pixel_bounds, const Vector2f &filter_radius,
           const Float *filter_table, int filter_table_size,
           Float max_sample_luminance)
      : pixel_bounds(pixel_bounds),
        filter_radius(filter_radius),
        inv_filter_radius(1 / filter_radius.x, 1 / filter_radius.y),
        filter_table(filter_table),
        filter_table_size(filter_table_size),
        max_sample_luminance(max_sample_luminance) {
    pixels = std::vector<FilmTilePixel>(std::max<int>(0, pixel_bounds.Area()));
  }

  void AddSample(const Point2f& pfilm, Spectrum l, Float sample_weight = 1.) {
    if (l.y > max_sample_luminance) l *= max_sample_luminance / l.y;
    Point2f film_discrete = pfilm - Vector2f(0.5f, 0.5f);
    Point2i p0 = (Point2i)Ceil(film_discrete - filter_radius);
    Point2i p1 = (Point2i)Floor(film_discrete + filter_radius) + Point2i(1, 1);
    p0 = Max(p0, pixel_bounds.pmin);
    p1 = Min(p1, pixel_bounds.pmax);
    int *ifx = new int(p1.x - p0.x);
    for (int x = p0.x; x < p1.x; ++x) {
      Float fx = std::abs((x - film_discrete.x) * inv_filter_radius.x *
          filter_table_size);
      ifx[x - p0.x] = std::min<int>((int)std::floor(fx), filter_table_size - 1);
    }
    int *ify = new int(p1.y - p0.y);
    for (int y = p0.y; y < p1.y; ++y) {
      Float fy = std::abs((y - film_discrete.y) * inv_filter_radius.y *
          filter_table_size);
      ify[y - p0.y] = std::min<int>((int)std::floor(fy), filter_table_size - 1);
    }
    for (int y = p0.y; y < p1.y; ++y) {
      for (int x = p0.x; x < p1.x; ++x) {
        // Evaluate filter value at $(x,y)$ pixel
        int offset = ify[y - p0.y] * filter_table_size + ifx[x - p0.x];
        Float filterWeight = filter_table[offset];

        // Update pixel values with filtered sample contribution
        FilmTilePixel &pixel = GetPixel(Point2i(x, y));
        pixel.contrib_sum += l * sample_weight * filterWeight;
        pixel.filter_weight_sum += filterWeight;
      }
    }
  }

  FilmTilePixel &GetPixel(const Point2i &p) {
    int width = pixel_bounds.pmax.x - pixel_bounds.pmin.x;
    int offset =
        (p.x - pixel_bounds.pmin.x) + (p.y - pixel_bounds.pmin.y) * width;
    return pixels[offset];
  }

  const FilmTilePixel &GetPixel(const Point2i &p) const {
    int width = pixel_bounds.pmax.x - pixel_bounds.pmin.x;
    int offset =
        (p.x - pixel_bounds.pmin.x) + (p.y - pixel_bounds.pmin.y) * width;
    return pixels[offset];
  }
  Bounds2i GetPixelBounds() const { return pixel_bounds; }
};

}
