#include "image.h"
#include <lodepng.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace min {

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
  }
  MIN_ERROR("Unable to load image {}", name);
  return nullptr;
}

}
