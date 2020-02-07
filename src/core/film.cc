#include <min-ray/film.h>

namespace min::ray {

Bounds2i Film::GetSampleBounds() const {
  Bounds2f floatBounds(Floor(Point2f(cropped_pixel_bounds.pMin) +
                           Vector2f(0.5f, 0.5f) - filter->radius),
                       Ceil(Point2f(cropped_pixel_bounds.pMax) -
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
  Point2i p0 = (Point2i)Ceil(floatBounds.pMin - halfPixel - filter->radius);
  Point2i p1 = (Point2i)Floor(floatBounds.pMax - halfPixel + filter->radius) +
      Point2i(1, 1);
  Bounds2i tilePixelBounds = Intersect(Bounds2i(p0, p1), cropped_pixel_bounds);
  return std::unique_ptr<FilmTile>(new FilmTile(
      tilePixelBounds, filter->radius, filter_table, filter_table_width,
      max_sample_luminance));
}

void Film::Clear() {
  for (Point2i p : cropped_pixel_bounds) {
    Pixel &pixel = GetPixel(p);
    for (int c = 0; c < 3; ++c)
      pixel.splatXYZ[c] = pixel.xyz[c] = 0;
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
    tilePixel.contrib_sum.ToXYZ(xyz);
    for (int i = 0; i < 3; ++i) mergePixel.xyz[i] += xyz[i];
    mergePixel.filter_weight_sum += tilePixel.filter_weight_sum;
  }
}

void Film::SetImage(const Spectrum *img) const {
  int nPixels = cropped_pixel_bounds.Area();
  for (int i = 0; i < nPixels; ++i) {
    Pixel &p = pixels[i];
    img[i].ToXYZ(p.xyz);
    p.filter_weight_sum = 1;
    p.splatXYZ[0] = p.splatXYZ[1] = p.splatXYZ[2] = 0;
  }
}

void Film::AddSplat(const Point2f &p, Spectrum v) {

  if (v.HasNaNs()) {
    MIN_ERROR("Ignoring splatted spectrum with NaN values "
              "at ({}, {})", p.x, p.y);
    return;
  } else if (v.y() < 0.) {
    MIN_ERROR("\"Ignoring splatted spectrum with negative \"\n"
              "                               \"luminance {} at ({}, {})\", v.y(), p.x, p.y");
    return;
  } else if (std::isinf(v.y())) {
    MIN_ERROR("Ignoring splatted spectrum with infinite "
                               "luminance at ({}, {})", p.x, p.y);
    return;
  }

  Point2i pi = Point2i(Floor(p));
  if (!InsideExclusive(pi, cropped_pixel_bounds)) return;
  if (v.y() > max_sample_luminance)
    v *= max_sample_luminance / v.y();
  Float xyz[3];
  v.ToXYZ(xyz);
  Pixel &pixel = GetPixel(pi);
  for (int i = 0; i < 3; ++i) pixel.splatXYZ[i].Add(xyz[i]);
}

void Film::WriteImage(Float splatScale) {
  // Convert image to RGB and compute final pixel values
  MIN_INFO("Converting image to RGB and computing final weighted pixel values");
  std::unique_ptr<Float[]> rgb(new Float[3 * cropped_pixel_bounds.Area()]);
  int offset = 0;
  for (Point2i p : cropped_pixel_bounds) {
    // Convert pixel XYZ color to RGB
    Pixel &pixel = GetPixel(p);
    XYZToRGB(pixel.xyz, &rgb[3 * offset]);

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

    // Add splat value at pixel
    Float splatRGB[3];
    Float splatXYZ[3] = {pixel.splatXYZ[0], pixel.splatXYZ[1],
                         pixel.splatXYZ[2]};
    XYZToRGB(splatXYZ, splatRGB);
    rgb[3 * offset] += splatScale * splatRGB[0];
    rgb[3 * offset + 1] += splatScale * splatRGB[1];
    rgb[3 * offset + 2] += splatScale * splatRGB[2];

    // Scale pixel value by _scale_
    rgb[3 * offset] *= scale;
    rgb[3 * offset + 1] *= scale;
    rgb[3 * offset + 2] *= scale;
    ++offset;
  }

  // Write RGB image
  MIN_INFO("Writing image {} with bounds {}", filename, cropped_pixel_bounds.ToString());
  //WriteImage(filename, &rgb[0], cropped_pixel_bounds, full_resolution);
}
}

