#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace cereal;
using namespace nlohmann;

class Object {
 public:
  virtual std::string GetType() const = 0;
  virtual void Initialize(json &json) const {}
};

class Integrator : public Object {
 public:
  int spp;
  template <class Archive>
  void serialize(Archive &archive) {
    archive(spp);
  }
  virtual std::string GetType() const { return "Integrator"; };
  virtual void Initialize(json &json) {
  }
};
class Camera : public Object {
 public:
  float fov;
  template <class Archive>
  void serialize(Archive &archive) {
    archive(fov);
  }
  virtual void Initialzie(json &json) {
    std::istream(json);
  }
  virtual std::string GetType() const { return "Camera"; }
};
class Graph : public Object {
 public:
  template <class Archive>
  void serialize(Archive &archive) {
    archive(camera, integrator);
  }
  void Initialize(json &json) const override {
    camera->Initialize(json.get(camera->GetType()));
    integrator->Initialize(json.get(integrator->GetType()));
  }
  virtual std::string GetType() const { return "Graph"; }
  std::shared_ptr<Camera> camera;
  std::shared_ptr<Integrator> integrator;
};
int main() {
  json j = {
      {"camera", {"fov", 80}},
      {"integrator", {"spp", 40}},
      //{"filedimension", {1080, 720}},
  };
  Graph graph;
  graph.Initialize(j);
  //std::cout << graph.camera->fov << std::endl;
  return 0;
}