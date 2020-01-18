
#include <min-ray/bitmap.h>
#include <min-ray/camera.h>
#include <min-ray/emitter.h>
#include <min-ray/integrator.h>
#include <min-ray/sampler.h>
#include <min-ray/scene.h>

namespace min::ray {

Scene::Scene(const PropertyList &) {
  m_accel = new Accel();
}

Scene::~Scene() {
  delete m_accel;
  delete m_sampler;
  delete m_camera;
  delete m_integrator;
  delete m_rendermode;
}

void Scene::activate() {
  m_accel->build();

  if (!m_integrator)
    throw NoriException("No integrator was specified!");
  if (!m_camera)
    throw NoriException("No camera was specified!");

  if (!m_sampler) {
    /* Create a default (independent) sampler */
    m_sampler = static_cast<Sampler *>(
        NoriObjectFactory::createInstance("independent", PropertyList()));
  }

  if (!m_rendermode) {
    /* Crate default render mode 'blockwise' */
    m_rendermode = static_cast<RenderMode *>(
        NoriObjectFactory::createInstance("blockwise", PropertyList()));
  }

  cout << endl;
  cout << "Configuration: " << toString() << endl;
  cout << endl;
}

void Scene::addChild(NoriObject *obj) {
  switch (obj->getClassType()) {
    case EMesh: {
      Mesh *mesh = static_cast<Mesh *>(obj);
      m_accel->addMesh(mesh);
      m_meshes.push_back(mesh);
    } break;

    case EEmitter: {
      //Emitter *emitter = static_cast<Emitter *>(obj);
      /* TBD */
      throw NoriException("Scene::addChild(): You need to implement this for emitters");
    } break;

    case ESampler:
      if (m_sampler)
        throw NoriException("There can only be one sampler per scene!");
      m_sampler = static_cast<Sampler *>(obj);
      break;

    case ECamera:
      if (m_camera)
        throw NoriException("There can only be one camera per scene!");
      m_camera = static_cast<Camera *>(obj);
      break;

    case EIntegrator:
      if (m_integrator)
        throw NoriException("There can only be one integrator per scene!");
      m_integrator = static_cast<Integrator *>(obj);
      break;

    case ERenderMode:
      if (m_rendermode)
        throw NoriException("There can only be one active render mode per scene!");
      m_rendermode = static_cast<RenderMode *>(obj);
      break;

    default:
      throw NoriException("Scene::addChild(<%s>) is not supported!",
                          classTypeName(obj->getClassType()));
  }
}

std::string Scene::toString() const {
  std::string meshes;
  for (size_t i = 0; i < m_meshes.size(); ++i) {
    meshes += std::string("  ") + indent(m_meshes[i]->toString(), 2);
    if (i + 1 < m_meshes.size())
      meshes += ",";
    meshes += "\n";
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
      indent(m_rendermode->toString()),
      indent(m_integrator->toString()),
      indent(m_sampler->toString()),
      indent(m_camera->toString()),
      indent(meshes, 2));
}

NORI_REGISTER_CLASS(Scene, "scene");
}  // namespace min::ray
