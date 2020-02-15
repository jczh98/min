#include "film.h"
#include "image.h"

namespace min {

void Film::initialize(const Json &json) {
  auto resolution = Value(json, "resolution", Point2i(800, 600));
  auto crop_window = Bounds2f(Point2f(0, 0), Point2f(1, 1));
  auto scal = Value(json, "scale", 1.0f);
  auto diagona = Value(json, "diagonal", 35.0f);
  auto max_sample_luminanc = Value(json, "max_sample_luminance", kInfinity);
  auto filenam = Value<std::string>(json, "filename", "");
  auto filt = CreateInstanceUnique<Filter>(json["filter"]["type"], GetProps(json["filter"]));
  Initialize(resolution, crop_window, std::move(filt), diagona, filenam, scal, max_sample_luminanc);
}

Bounds2i Film::GetSampleBounds() const {
  Bounds2f floatBounds(Floor(Point2f(cropped_pixel_bounds.pmin) +
                           Vector2f(0.5f, 0.5f) - filter->radius),
                       Ceil(Point2f(cropped_pixel_bounds.pmax) -
                           Vector2f(0.5f, 0.5f) + filter->radius));
  return (Bounds2i)floatBounds;
}

Bounds2f Film::GetPhysicalExtent() const {
  Float aspect = (Float)full_resolution.y / (Float)full_resolution.x;
  Float x = std::sqrt(diagonal * diagonal / (1 + aspect * aspect));
  Float y = aspect * x;
  return Bounds2f(Point2f(-x / 2, -y / 2), Point2f(x / 2, y / 2));
}

std::unique_ptr<FilmTile> Film::GetFilmTile(const Bounds2i &sampleBounds) {
  // Bound image pixels that samples in _sampleBounds_ contribute to
  Vector2f halfPixel = Vector2f(0.5f, 0.5f);
  Bounds2f floatBounds = (Bounds2f)sampleBounds;
  Point2i p0 = (Point2i)Ceil(floatBounds.pmin - halfPixel - filter->radius);
  Point2i p1 = (Point2i)Floor(floatBounds.pmax - halfPixel + filter->radius) +
      Point2i(1, 1);
  Bounds2i tilePixelBounds = Intersect(Bounds2i(p0, p1), cropped_pixel_bounds);
  return std::unique_ptr<FilmTile>(new FilmTile(
      tilePixelBounds, filter->radius, filter_table, filter_table_width,
      max_sample_luminance));
}

void Film::Clear() {
  for (Point2i p : cropped_pixel_bounds) {
    Pixel &pixel = GetPixel(p);
    pixel.value = Spectrum(0.f);
    pixel.filter_weight_sum = 0;
  }
}

void Film::MergeFilmTile(std::unique_ptr<FilmTile> tile) {
  std::lock_guard<std::mutex> lock(mutex);
  for (Point2i pixel : tile->GetPixelBounds()) {
    // Merge _pixel_ into _Film::pixels_
    const FilmTilePixel &tilePixel = tile->GetPixel(pixel);
    Pixel &mergePixel = GetPixel(pixel);
    Float xyz[3];
    for (int i = 0; i < 3; ++i) mergePixel.value += tilePixel.contrib_sum;
    mergePixel.filter_weight_sum += tilePixel.filter_weight_sum;
  }
}

void Film::SetImage(Spectrum *img) const {
  int nPixels = cropped_pixel_bounds.Area();
  for (int i = 0; i < nPixels; ++i) {
    Pixel &p = pixels[i];
    img[i] = p.value;
    p.filter_weight_sum = 1;
  }
}

void Film::WriteImage(Float splatScale) {
  // Convert image to RGB and compute final pixel values
  MIN_INFO("Converting image to RGB and computing final weighted pixel values");
  std::unique_ptr<Float[]> rgb(new Float[3 * cropped_pixel_bounds.Area()]);
  int offset = 0;
  for (Point2i p : cropped_pixel_bounds) {
    // Convert pixel XYZ color to RGB
    Pixel &pixel = GetPixel(p);

    rgb[3 * offset] = pixel.value.x;
    rgb[3 * offset + 1] = pixel.value.y;
    rgb[3 * offset + 2] = pixel.value.z;

    // Normalize pixel with weight sum
    Float filter_weight_sum = pixel.filter_weight_sum;
    if (filter_weight_sum != 0) {
      Float invWt = (Float)1 / filter_weight_sum;
      rgb[3 * offset] = std::max<Float>((Float)0, rgb[3 * offset] * invWt);
      rgb[3 * offset + 1] =
          std::max<Float>((Float)0, rgb[3 * offset + 1] * invWt);
      rgb[3 * offset + 2] =
          std::max<Float>((Float)0, rgb[3 * offset + 2] * invWt);
    }

    // Scale pixel value by _scale_
    rgb[3 * offset] *= scale;
    rgb[3 * offset + 1] *= scale;
    rgb[3 * offset + 2] *= scale;
    ++offset;
  }

  // Write RGB image
  MIN_INFO("Writing image {} with bounds {}", filename, cropped_pixel_bounds.ToString());
  min::WriteImage(filename, &rgb[0], cropped_pixel_bounds, full_resolution);
}
MIN_IMPLEMENTATION(Film, Film, "film")

}

