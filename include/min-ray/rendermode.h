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
  virtual void Render(Scene *scene, const std::string &filename) = 0;

  EClassType getClassType() const { return ERenderMode; }

 private:
  /* nothing to-do here */
};

}  // namespace min::ray
