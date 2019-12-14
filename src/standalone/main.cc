#include <min-ray/graph.h>
#include <iostream>

int main(int, char**) {
  auto graph = std::make_shared<min::ray::SceneGraph>();
  graph->Initialize();
  graph->Render();
  return 0;
}
