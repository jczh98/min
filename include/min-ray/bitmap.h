#pragma once

#include <min-ray/color.h>
#include <min-ray/vector.h>

namespace min::ray {

/**
 * \brief Stores a RGB high dynamic-range bitmap
 *
 * The bitmap class provides I/O support using the OpenEXR file format
 */
class Bitmap : public Eigen::Array<Color3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> {
 public:
  typedef Eigen::Array<Color3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Base;

  /**
     * \brief Allocate a new bitmap of the specified size
     *
     * The contents will initially be undefined, so make sure
     * to call \ref clear() if necessary
     */
  Bitmap(const Vector2i &size = Vector2i(0, 0))
      : Base(size.y(), size.x()) {}

  /// Load an OpenEXR file with the specified filename
  Bitmap(const std::string &filename);

  /// Save the bitmap as an EXR file with the specified filename
  void save(const std::string &filename);
};

}  // namespace min::ray
