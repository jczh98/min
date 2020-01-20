
#include <min-ray/bitmap.h>
#include <min-ray/camera.h>
#include <min-ray/emitter.h>
#include <min-ray/integrator.h>
#include <min-ray/sampler.h>
#include <min-ray/scene.h>

namespace min::ray {

Scene::Scene(const PropertyList &) {
  accelerator = new Accel();
}

Scene::~Scene() {
  delete accelerator;
  delete sampler;
  delete camera;
  delete integrator;
  delete rendermode;
}

void Scene::activate() {
  accelerator->Build();

  if (!integrator)
    throw NoriException("No integrator was specified!");
  if (!camera)
    throw NoriException("No camera was specified!");

  if (!sampler) {
    /* Create a default (independent) sampler */
    sampler = static_cast<Sampler *>(
        NoriObjectFactory::createInstance("independent", PropertyList()));
  }

  if (!rendermode) {
    /* Crate default render mode 'blockwise' */
    rendermode = static_cast<RenderMode *>(
        NoriObjectFactory::createInstance("blockwise", PropertyList()));
  }

  cout << endl;
  cout << "Configuration: " << ToString() << endl;
  cout << endl;
}

void Scene::addChild(NoriObject *obj) {
  switch (obj->getClassType()) {
    case EMesh: {
      Mesh *mesh = static_cast<Mesh *>(obj);
      accelerator->AddMesh(mesh);
      meshes.push_back(mesh);
      if (mesh->IsLight()) {
        lights.emplace_back( mesh->GetEmitter());
      }
    } break;

    case EEmitter: {
      Emitter *emitter = static_cast<Emitter *>(obj);
      lights.emplace_back(emitter);
    } break;

    case ESampler:
      if (sampler)
        throw NoriException("There can only be one sampler per scene!");
      sampler = static_cast<Sampler *>(obj);
      break;

    case ECamera:
      if (camera)
        throw NoriException("There can only be one camera per scene!");
      camera = static_cast<Camera *>(obj);
      break;

    case EIntegrator:
      if (integrator)
        throw NoriException("There can only be one integrator per scene!");
      integrator = static_cast<Integrator *>(obj);
      break;

    case ERenderMode:
      if (rendermode)
        throw NoriException("There can only be one active render mode per scene!");
      rendermode = static_cast<RenderMode *>(obj);
      break;

    default:
      throw NoriException("Scene::addChild(<%s>) is not supported!",
                          classTypeName(obj->getClassType()));
  }
}

std::string Scene::ToString() const {
  std::string mesh_string;
  for (size_t i = 0; i < this->meshes.size(); ++i) {
      mesh_string += std::string("  ") + indent(this->meshes[i]->ToString(), 2);
    if (i + 1 < this->meshes.size())
        mesh_string += ",";
    mesh_string += "\n";
  }

  return tfm::format(
      "Scene[\n"
      "  rendermode = %s, \n"
      "  integrator = %s,\n"
      "  sampler = %s\n"
      "  camera = %s,\n"
      "  meshes = {\n"
      "  %s  }\n"
      "]",
      indent(rendermode->ToString()),
      indent(integrator->ToString()),
      indent(sampler->ToString()),
      indent(camera->ToString()),
      indent(mesh_string, 2));
}

NORI_REGISTER_CLASS(Scene, "scene");
}  // namespace min::ray
