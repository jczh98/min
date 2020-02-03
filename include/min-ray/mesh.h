
#pragma once

#include <min-ray/interface.h>
#include <min-ray/bbox.h>
#include <min-ray/frame.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>
#include <min-ray/dpdf.h>

namespace min::ray {

/**
 * \brief Intersection data structure
 *
 * This data structure records local information about a ray-triangle intersection.
 * This includes the position, traveled ray distance, uv coordinates, as well
 * as well as two local coordinate frames (one that corresponds to the true
 * geometry, and one that is used for shading computations).
 */
struct Intersection {
  /// Position of the surface intersection
  Point3f p;
  /// Unoccluded distance along the ray
  float t;
  /// UV coordinates, if any
  Point2f uv;
  /// Shading frame (based on the shading normal)
  Frame shading_frame;
  /// Geometric frame (based on the true geometry)
  Frame geo_frame;
  /// Pointer to the associated mesh
  const Mesh *mesh;

  /// Create an uninitialized intersection record
  Intersection() : mesh(nullptr) {}

  /// Transform a direction vector into the local shading frame
  Vector3f toLocal(const Vector3f &d) const {
    return shading_frame.ToLocal(d);
  }

  /// Transform a direction vector from local to world coordinates
  Vector3f toWorld(const Vector3f &d) const {
    return shading_frame.ToWorld(d);
  }

  /// Return a human-readable summary of the intersection record
  //std::string ToString() const;
};

struct SurfaceSample {
  // Reference point
  Point3f ref;
  // Sampled point
  Point3f p;
  // Sampled normal
  Normal3f n;
  // Probability of the sample
  float pdf;

  /// Empty constructor
  SurfaceSample() {}
  /// Data structure with ref to call sampleSurface()
  SurfaceSample(const Point3f & ref_) : ref(ref_) {}
  /// Data structure with ref and p to call pdfSurface()
  SurfaceSample(const Point3f & ref_, const Point3f & p_) : ref(ref_), p(p_) {}

};

/**
 * \brief Triangle mesh
 *
 * This class stores a triangle mesh object and provides numerous functions
 * for querying the individual triangles. Subclasses of \c Mesh implement
 * the specifics of how to create its contents (e.g. by loading from an
 * external file)
 */
class Mesh : public Unit {
 public:

  /// Initialize internal data structures (called once by the XML parser)
  //virtual void activate();

  /// Return the total number of triangles in this shape
  uint32_t GetTriangleCount() const { return (uint32_t)faces.cols(); }

  /// Return the total number of vertices in this shape
  uint32_t GetVertexCount() const { return (uint32_t)positions.cols(); }

  /**
     * \brief Uniformly sample a position on the mesh with
     * respect to surface area. Returns both position and normal
     */
  void Sample(const Point2f &sample, SurfaceSample &surface_sample) const;

  float Pdf(const SurfaceSample &surface_sample) const;

  /// Return the surface area of the given triangle
  float surface_area(uint32_t index) const;

  //// Return an axis-aligned bounding box of the entire mesh
  const BoundingBox3f &GetBoundingBox() const { return bound_box; }

  //// Return an axis-aligned bounding box containing the given triangle
  BoundingBox3f GetBoundingBox(uint32_t index) const;

  //// Return the centroid of the given triangle
  Point3f centroid(uint32_t index) const;

  /** \brief Ray-triangle intersection test
     *
     * Uses the algorithm by Moeller and Trumbore discussed at
     * <tt>http://www.acm.org/jgt/papers/MollerTrumbore97/code.html</tt>.
     *
     * Note that the test only applies to a single triangle in the mesh.
     * An acceleration data structure like \ref BVH is needed to search
     * for intersections against many triangles.
     *
     * \param index
     *    Index of the triangle that should be intersected
     * \param ray
     *    The ray segment to be used for the intersection query
     * \param t
     *    Upon success, \a t contains the distance from the ray origin to the
     *    intersection point,
     * \param u
     *   Upon success, \c u will contain the 'U' component of the intersection
     *   in barycentric coordinates
     * \param v
     *   Upon success, \c v will contain the 'V' component of the intersection
     *   in barycentric coordinates
     * \return
     *   \c true if an intersection has been detected
     */
  bool Intersect(uint32_t index, const Ray3f &ray, float &u, float &v, float &t) const;

  Point3f GetInterpolatedVertex(uint32_t index, const Vector3f & bc) const;
  Normal3f GetInterpolatedNormal(uint32_t index, const Vector3f & bc) const;

  /// Return a pointer to the vertex positions
  const MatrixXf &GetVertexPositions() const { return positions; }

  /// Return a pointer to the vertex normals (or \c nullptr if there are none)
  const MatrixXf &GetVertexNormals() const { return normals; }

  /// Return a pointer to the texture coordinates (or \c nullptr if there are none)
  const MatrixXf &GetVertexTexCoords() const { return texcoords; }

  /// Return a pointer to the triangle vertex index list
  const MatrixXu &GetIndices() const { return faces; }

  /// Is this mesh an area emitter?
  bool IsLight() const { return light != nullptr; }

  std::shared_ptr<Emitter> GetLight() { return light; }

  const std::shared_ptr<Emitter> GetLight() const { return light; }

  /// Return a pointer to an attached area emitter instance
  Emitter *GetEmitter() { return light.get(); }

  /// Return a pointer to an attached area emitter instance (const version)
  const Emitter *GetEmitter() const { return light.get(); }

  /// Return a pointer to the BSDF associated with this mesh
  const BSDF *GetBSDF() const { return bsdf.get(); }

  /// Register a child object (e.g. a BSDF) with the mesh
  //virtual void addChild(NoriObject *child);

  /// Return the name of this mesh
  const std::string &GetName() const { return name_val; }

  /// Return a human-readable summary of this instance
  //std::string ToString() const;
  void initialize(const json &json) override;

  float total_surface_area = 0.0f;

 protected:
  std::string name_val;            ///< Identifying name
  MatrixXf positions;                  ///< Vertex positions
  MatrixXf normals;                  ///< Vertex normals
  MatrixXf texcoords;                 ///< Vertex texture coordinates
  MatrixXu faces;                  ///< Faces
  std::shared_ptr<BSDF> bsdf = nullptr;        ///< BSDF of the surface
  std::shared_ptr<Emitter> light = nullptr;  ///< Associated emitter, if any
  DiscretePDF dpdf;
  BoundingBox3f bound_box;          ///< Bounding box of the mesh
};
MIN_INTERFACE(Mesh)

}  // namespace min::ray
