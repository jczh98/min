#pragma once

#include <min/math/math.h>
#include <min/common/interface.h>
#include "defs.h"
#include "geometry.h"
#include "transform.h"
#include "film.h"

namespace min {

class Camera : public Unit {
 protected:
  Transform camera2world;
 public:
  virtual Float GenerateRay(const Point2f &pfilm, const Point2f &plens, Float time, Ray &ray) const = 0;
  std::shared_ptr<Film> film;
};
MIN_INTERFACE(Camera)

}


