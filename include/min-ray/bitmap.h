#pragma once

#include <min-ray/color.h>
#include <min-ray/vector.h>

namespace min::ray {

class Bitmap : public Eigen::Array<Color3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> {
 public:
  typedef Eigen::Array<Color3f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> Base;

  Bitmap(const Vector2i &size = Vector2i(0, 0))
      : Base(size.y(), size.x()) {}

  Bitmap(const std::string &filename);

  void Save(const std::string &filename);
};

}  // namespace min::ray
