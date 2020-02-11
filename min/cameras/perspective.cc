#include <min/visual/camera.h>

namespace min {

class PerspectiveCamera : public Camera {
 public:
  void initialize(const Json &json) override {

  }
  Float GenerateRay(const Point2f &pfilm, const Point2f &flens, Float time, Ray &ray) const override {
    return 0;
  }
};
MIN_IMPLEMENTATION(Camera, PerspectiveCamera, "perspective")

}

