#include <min/visual/texture.h>
#include <min/visual/image.h>
#include <min/visual/mipmap.h>

namespace min {

class ImageTexture : public Texture {
  std::unique_ptr<MIPMap> mipmap = nullptr;
  int width, height;
 public:
  void initialize(const Json &json) override {
    auto filename = GetFileResolver()->Resolve(json["filename"].get<std::string>());
    auto wrap = Value<std::string>(json, "warp_mode", "repeat");
    auto scale = Value(json, "scale", 1.f);
    auto wm = ImageWrapMode::kRepeat;
    if (wrap == "black") wm = ImageWrapMode::kBlack;
    if (wrap == "clamp") wm = ImageWrapMode::kClamp;
    auto gamma = Value(json, "gamma", HasExtension(filename.string(), "png"));

    // Create MIPMap
    Point2i resolution;
    std::unique_ptr<Vector3[]> texels = ReadImage(filename.string(), resolution.x, resolution.y);
    if (!texels) {
      MIN_WARN("Creating a constant grey texture to replace \"{}\".", filename.string());
      resolution.x = resolution.y = 1;
      texels.reset(new Vector3(0.5));
    }
    // Flip image in y;
    for (int y = 0; y < resolution.y / 2; y++) {
      for (int x = 0; x < resolution.x; x++) {
        int o1 = y * resolution.x + x;
        int o2 = (resolution.y - 1 - y) * resolution.x + x;
        std::swap(texels[o1], texels[o2]);
      }
    }
    if (texels) {
      std::unique_ptr<Vector3[]> converted_texels(new Vector3[resolution.x * resolution.y]);
      for (int i = 0; i < resolution.x * resolution.y; i++) {
        for (int j = 0; j < 3; j++) {
          converted_texels[i][j] = scale * (gamma ? InverseGammaCorrect(texels[i][j]) : texels[i][j]);
        }
      }
      mipmap.reset(new MIPMap(resolution, converted_texels.get(), wm));
    } else {
      Vector3 val(scale);
      mipmap.reset(new MIPMap(Point2i(1, 1), &val));
    }
  }
  Spectrum Evaluate(const ShadingPoint &sp) const override {
    return mipmap->Lookup(sp.texcoords);
  }
};
MIN_IMPLEMENTATION(Texture, ImageTexture, "image");

}

