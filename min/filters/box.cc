#include <min/visual/filter.h>

namespace min {

class BoxFilter : public Filter {
 public:
  void initialize(const Json &json) override {
    radius = Value(json, "radius", Vector2f(0.5, 0.5));
  }

  Float Evaluate(const Point2 &p) const override {
    return 1;
  }
};
MIN_IMPLEMENTATION(Filter, BoxFilter, "box")

}

