#include "../src/core/importers/wavefront.h"
#include "../src/core/shapes/triangle.h"

using namespace min::ray;
int main(int, char**) {
  auto importer = std::make_shared<WavefrontImporter>();
  auto result = importer->Import(fs::path("assets/fireplace_room/fireplace_room.obj"));
  return 0;
}