#include <min-ray/texture.h>

namespace min::ray {
class ConstantTexture final : public Texture {
 public:
  Spectrum Evaluate(const ShadingPoint &sp) const override {
		return Spectrum(0);
	}
};
MIN_IMPLEMENTATION(Texture, ConstantTexture, "constant")
}  // namespace min::ray