#include <min/visual/aggregate.h>
#include "sphere.h"

namespace min {

class SphereInstance : public Aggregate {
 public:
  void initialize(const Json &json) override {
    auto radius = Value(json, "radius", 1.0);
    auto center = Value(json, "center", Vector3f(0));
    std::shared_ptr<Material> material = nullptr;
    if (json.contains("material")) {
      material = CreateInstance<Material>(json["material"]["type"], GetProps(json["material"]));
    } else {
      material = CreateInstance<Material>("diffuse", {});
    }
    auto shape = std::make_shared<Sphere>(Transform(), Transform(), radius, center);
    shape->material = material;
    shapes.emplace_back(shape);

  }
};
MIN_IMPLEMENTATION(Aggregate, SphereInstance, "sphere")

}

