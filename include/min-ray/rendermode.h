#pragma once

#include <min-ray/interface.h>
#include <min-ray/object.h>

namespace min::ray {

/**
 * \brief RenderMode handles different render modes
 *
 * This class serves as interface for new render modes
 * In this example we implemented a progressive render mode for a faster visualization
 * (default: blockwise, new: progressive)
 */
class RenderMode : public Unit {
 public:
  /// Release all memory
  virtual ~RenderMode() {}

  /// Main render process
  virtual void Render(Scene *scene, const std::string &filename) = 0;

 private:
  /* nothing to-do here */
};
MIN_INTERFACE(RenderMode)

}  // namespace min::ray
