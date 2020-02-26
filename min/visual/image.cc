#include "image.h"
#include <lodepng.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <OpenEXR/ImfRgba.h>
#include <OpenEXR/half.h>
#include <OpenEXR/ImfRgbaFile.h>

namespace min {

static Vector3 *ReadImageEXR(const std::string &name, int &width, int &height,
                          Bounds2i *dataWindow = nullptr, Bounds2i *displayWindow = nullptr) {
  using namespace Imf;
  using namespace Imath;
  try {
    RgbaInputFile file(name.c_str());
    Box2i dw = file.dataWindow();

    // OpenEXR uses inclusive pixel bounds; adjust to non-inclusive
    // (the convention pbrt uses) in the values returned.
    if (dataWindow)
      *dataWindow = Bounds2i(Point2i(dw.min.x, dw.min.y), Point2i{dw.max.x + 1, dw.max.y + 1});
    if (displayWindow) {
      Box2i dispw = file.displayWindow();
      *displayWindow = Bounds2i{Point2i{dispw.min.x, dispw.min.y},
                                Point2i{dispw.max.x + 1, dispw.max.y + 1}};
    }
    width = dw.max.x - dw.min.x + 1;
    height = dw.max.y - dw.min.y + 1;

    std::vector<Rgba> pixels(width * height);
    file.setFrameBuffer(&pixels[0] - dw.min.x - dw.min.y * width, 1,
                        width);
    file.readPixels(dw.min.y, dw.max.y);

    Vector3 *ret = new Vector3[width * height];
    for (int i = 0; i < width * height; ++i) {
      Float frgb[3] = {pixels[i].r, pixels[i].g, pixels[i].b};
      ret[i] = Vector3(frgb[0], frgb[1], frgb[2]);
    }
    MIN_INFO("Read EXR image {} ({} x {})", name, width, height);
    return ret;
  } catch (const std::exception &e) {
    MIN_ERROR("Unable to read image file \"{}\": {}", name, e.what());
  }

  return NULL;
}


static Vector3 *ReadImagePNG(const std::string &name, int &width, int &height) {
  unsigned char *rgb;
  unsigned w, h;
  uint error = lodepng_decode24_file(&rgb, &w, &h, name.c_str());
  if (error != 0) {
    MIN_ERROR("Error reading PNG \"{}\": {}", name, lodepng_error_text(error));
    return nullptr;
  }
  width = w; height = h;
  Vector3 *ret = new Vector3[width * height];
  unsigned char *src = rgb;
  for (uint y = 0; y < h; y++) {
    for (uint x = 0; x < w; x++, src += 1) {
      ret[y * width + x] = Vector3(src[0] / 255.f, src[1] / 255.f, src[2] / 255.f);
    }
  }
  free(rgb);
  MIN_INFO("Read PNG image {} ({} x {})", name, width, height);
  return ret;
}

inline bool HasExtension(const std::string &value, const std::string &ending) {
  if (ending.size() > value.size()) {
    return false;
  }
  return std::equal(
      ending.rbegin(), ending.rend(), value.rbegin(),
      [](char a, char b) { return std::tolower(a) == std::tolower(b); });
}

void WriteImage(const std::string &name,
                const Float *rgb,
                const Bounds2i &output_bounds,
                const Point2i &total_resolution) {
  if (HasExtension(name, "png")) {
    Vector2i resolution = output_bounds.Diagonal();
    std::unique_ptr<uint8_t[]> rgb8(new uint8_t[3 * resolution.x * resolution.y]);
    uint8_t *dst = rgb8.get();
    for (int y = 0; y < resolution.y; ++y) {
      for (int x = 0; x < resolution.x; ++x) {
#define TO_BYTE(v) (uint8_t) Clamp(255.f * GammaCorrect(v), 0.f, 255.f)
        dst[0] = TO_BYTE(rgb[3 * (y * resolution.x + x) + 0]);
        dst[1] = TO_BYTE(rgb[3 * (y * resolution.x + x) + 1]);
        dst[2] = TO_BYTE(rgb[3 * (y * resolution.x + x) + 2]);
#undef TO_BYTE
        dst += 3;
      }
    }
    stbi_write_png(name.c_str(), total_resolution.x, total_resolution.y, 3, rgb8.get(), 3 * resolution.x);
  }
}

std::unique_ptr<Vector3[]> ReadImage(const std::string &name, int &width, int &height) {
  if (HasExtension(name, "png")) {
    return std::unique_ptr<Vector3[]>(ReadImagePNG(name, width, height));
  } else if (HasExtension(name, "exr")) {
    return std::unique_ptr<Vector3[]>(ReadImageEXR(name, width, height));
  }
  MIN_ERROR("Unable to load image {}", name);
  return nullptr;
}

}
