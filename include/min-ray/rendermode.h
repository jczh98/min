#include <min-ray/object.h>

namespace min::ray {

/**
 * \brief RenderMode handles different render modes
 *
 * This class serves as interface for new render modes
 * In this example we implemented a progressive render mode for a faster visualization
 * (default: blockwise, new: progressive)
 */
class RenderMode : public NoriObject {
 public:
  /// Release all memory
  virtual ~RenderMode() {}

  /// Main render process
  virtual void render(Scene *scene, const std::string &filename) = 0;

  /// Sub-Render process for one block
  virtual void renderBlock(const Scene *scene, Sampler *sampler, ImageBlock &block) = 0;

  EClassType getClassType() const { return ERenderMode; }

 private:
  /* nothing to-do here */
};

}  // namespace min::ray
