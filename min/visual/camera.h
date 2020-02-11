#pragma once

#include <min/math/math.h>
#include <min/common/interface.h>
#include "defs.h"
#include "geometry.h"

namespace min {

class Camera : public Unit {
 public:
  virtual Float GenerateRay(const Point2f &pfilm, const Point2f &plens, Float time, Ray &ray) const = 0;
  void SetFilm(const std::shared_ptr<Film> &film) { this->film = film; }
  std::shared_ptr<Film> film;
};
MIN_INTERFACE(Camera)

}


