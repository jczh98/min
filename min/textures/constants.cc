#include <min/visual/texture.h>

namespace min {

class FloatTexture : public Texture {
  Float value = 1;
 public:
  void initialize(const Json &json) override {
    value = Value(json, "value", 0.5f);
  }
  Spectrum Evaluate(const ShadingPoint &sp) const override {
    return Spectrum(value);
  }
};
MIN_IMPLEMENTATION(Texture, FloatTexture, "float")

class ConstantTexture : public Texture {
  Spectrum value;
 public:
  void initialize(const Json &json) override {
    value = Value(json, "value", Spectrum(0.f));
  }
  Spectrum Evaluate(const ShadingPoint &sp) const override {
    return value;
  }
};
MIN_IMPLEMENTATION(Texture, ConstantTexture, "constant")

}

