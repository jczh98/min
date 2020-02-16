#include <min/visual/aggregate.h>
#include "sphere.h"

namespace min {

class SphereInstance : public Aggregate {
 public:
  void initialize(const Json &json) override {
    auto radius = Value(json, "radius", 1.0);
    auto center = Value(json, "center", Vector3f(0));
    shapes.emplace_back(std::make_shared<Sphere>(Transform(), Transform(), radius, center));
  }
};
MIN_IMPLEMENTATION(Aggregate, SphereInstance, "sphere")

}

