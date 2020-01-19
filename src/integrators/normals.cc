#include <min-ray/integrator.h>
#include <min-ray/scene.h>

namespace min::ray {

class NormalIntegrator : public Integrator {
 public:
  NormalIntegrator(const PropertyList &props) {
    /* No parameters this time */
  }

  Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {
    /* Find the surface that is visible in the requested direction */
    Intersection its;
    if (!scene->Intersect(ray, its))
      return Color3f(0.0f);

    /* Return the component-wise absolute
           value of the shading normal as a color */
    Normal3f n = its.shading_frame.n.cwiseAbs();
    return Color3f(n.x(), n.y(), n.z());
  }

  std::string ToString() const {
    return "NfrmalIntegrator[]";
  }
};

NORI_REGISTER_CLASS(NormalIntegrator, "normals");

}