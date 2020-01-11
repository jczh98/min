#include <min-ray/texture.h>

namespace min::ray {
class ConstantTexture final : public Texture {
 public:
};
MIN_IMPLEMENTION(Texture, ConstantTexture, "constant")
}  // namespace min::ray