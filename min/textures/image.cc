#include <min/visual/texture.h>
#include <min/visual/image.h>

namespace min {

class ImageTexture : public Texture {
  std::unique_ptr<Vector3[]> image;
  int width, height;
 public:
  void initialize(const Json &json) override {
    auto filename = GetFileResolver()->Resolve(json["filename"].get<std::string>());
    image = ReadImage(filename.string(), width, height);
  }
  Spectrum Evaluate(const ShadingPoint &sp) const override {
    if (image) {
      int u = (sp.texcoords.x - std::floor(sp.texcoords.x)) * width;
      int v = ((1 - sp.texcoords.y) - std::floor(1 - sp.texcoords.y)) * height;
      auto color = image[v * width + u];
      return Spectrum(color);
    }
    return Spectrum(0);
  }
};
MIN_IMPLEMENTATION(Texture, ImageTexture, "image");

}

