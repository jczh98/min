
#pragma once

#include <min-ray/accel.h>
#include <min-ray/rendermode.h>

namespace min::ray {

/**
 * \brief Main scene data structure
 *
 * This class holds information on scene objects and is responsible for
 * coordinating rendering jobs. It also provides useful query routines that
 * are mostly used by the \ref Integrator implementations.
 */
class Scene : public NoriObject {
 public:
  /// Construct a new scene object
  Scene(const PropertyList &);

  /// Release all memory
  virtual ~Scene();

  /// Return a pointer to the scene's kd-tree
  const Accel *GetAccelerator() const { return accelerator; }

  /// Return a pointer to the scene's integrator
  const Integrator *GetIntegrator() const { return integrator; }

  /// Return a pointer to the scene's integrator
  Integrator *GetIntegrator() { return integrator; }

  /// Return a pointer to the scene's camera
  const Camera *GetCamera() const { return camera; }

  /// Return a pointer to the scene's sample generator (const version)
  const Sampler *GetSampler() const { return sampler; }

  /// Return a pointer to the scene's sample generator
  Sampler *GetSampler() { return sampler; }

  /// Return a pointer to the scene's render mode
  RenderMode *GetRenderMode() { return rendermode; }

  /// Return a reference to an array containing all meshes
  const std::vector<Mesh *> &GetMeshes() const { return meshes; }

  const std::vector<Emitter *> &GetLights() const { return lights; }
  /**
     * \brief Intersect a ray against all triangles stored in the scene
     * and return detailed intersection information
     *
     * \param ray
     *    A 3-dimensional ray data structure with minimum/maximum
     *    extent information
     *
     * \param its
     *    A detailed intersection record, which will be filled by the
     *    intersection query
     *
     * \return \c true if an intersection was found
     */
  bool Intersect(const Ray3f &ray, Intersection &its) const {
    return accelerator->Intersect(ray, its, false);
  }

  /**
     * \brief Intersect a ray against all triangles stored in the scene
     * and \a only determine whether or not there is an intersection.
     *
     * This method much faster than the other ray tracing function,
     * but the performance comes at the cost of not providing any
     * additional information about the detected intersection
     * (not even its position).
     *
     * \param ray
     *    A 3-dimensional ray data structure with minimum/maximum
     *    extent information
     *
     * \return \c true if an intersection was found
     */
  bool Intersect(const Ray3f &ray) const {
    Intersection its; /* Unused */
    return accelerator->Intersect(ray, its, true);
  }

  /// \brief Return an axis-aligned box that bounds the scene
  const BoundingBox3f &getBoundingBox() const {
    return accelerator->GetBoundingBox();
  }

  /**
     * \brief Inherited from \ref NoriObject::activate()
     *
     * Initializes the internal data structures (kd-tree,
     * emitter sampling data structures, etc.)
     */
  void activate();

  /// Add a child object to the scene (meshes, integrators etc.)
  void addChild(NoriObject *obj);

  /// Return a string summary of the scene (for debugging purposes)
  std::string ToString() const;

  EClassType getClassType() const { return EScene; }

 private:
  std::vector<Mesh *> meshes;
  std::vector<Emitter *> lights;
  Integrator *integrator = nullptr;
  Sampler *sampler = nullptr;
  Camera *camera = nullptr;
  Accel *accelerator = nullptr;
  RenderMode *rendermode = nullptr;
};

}  // namespace min::ray
