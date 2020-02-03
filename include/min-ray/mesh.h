
#pragma once

#include <min-ray/interface.h>
#include <min-ray/bbox.h>
#include <min-ray/frame.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>
#include <min-ray/dpdf.h>

namespace min::ray {

struct Intersection {
  // Position of the surface intersection
  Point3f p;
  // Unoccluded distance along the ray
  float t;
  // UV coordinates, if any
  Point2f uv;
  // Shading frame (based on the shading normal)
  Frame shading_frame;
  // Geometric frame (based on the true geometry)
  Frame geo_frame;
  // Pointer to the associated mesh
  const Mesh *mesh;

  // Create an uninitialized intersection record
  Intersection() : mesh(nullptr) {}

  // Transform a direction vector into the local shading frame
  Vector3f toLocal(const Vector3f &d) const {
    return shading_frame.ToLocal(d);
  }

  // Transform a direction vector from local to world coordinates
  Vector3f toWorld(const Vector3f &d) const {
    return shading_frame.ToWorld(d);
  }
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

  // Empty constructor
  SurfaceSample() {}
  // Data structure with ref to call sampleSurface()
  SurfaceSample(const Point3f & ref_) : ref(ref_) {}
  // Data structure with ref and p to call pdfSurface()
  SurfaceSample(const Point3f & ref_, const Point3f & p_) : ref(ref_), p(p_) {}

};

class Mesh : public Unit {
 public:

  uint32_t GetTriangleCount() const { return (uint32_t)faces.cols(); }

  uint32_t GetVertexCount() const { return (uint32_t)positions.cols(); }

  void Sample(const Point2f &sample, SurfaceSample &surface_sample) const;

  float Pdf(const SurfaceSample &surface_sample) const;

  // Return the surface area of the given triangle
  float surface_area(uint32_t index) const;

  // Return an axis-aligned bounding box of the entire mesh
  const BoundingBox3f &GetBoundingBox() const { return bound_box; }

  // Return an axis-aligned bounding box containing the given triangle
  BoundingBox3f GetBoundingBox(uint32_t index) const;

  // Return the centroid of the given triangle
  Point3f centroid(uint32_t index) const;

  bool Intersect(uint32_t index, const Ray3f &ray, float &u, float &v, float &t) const;

  Point3f GetInterpolatedVertex(uint32_t index, const Vector3f & bc) const;
  Normal3f GetInterpolatedNormal(uint32_t index, const Vector3f & bc) const;

  const MatrixXf &GetVertexPositions() const { return positions; }

  const MatrixXf &GetVertexNormals() const { return normals; }

  const MatrixXf &GetVertexTexCoords() const { return texcoords; }

  const MatrixXu &GetIndices() const { return faces; }

  bool IsLight() const { return light != nullptr; }

  std::shared_ptr<Light> GetLight() { return light; }

  const std::shared_ptr<Light> GetLight() const { return light; }

  const BSDF *GetBSDF() const { return bsdf.get(); }

  const std::string &GetName() const { return name_val; }

  void initialize(const json &json) override;

  float total_surface_area = 0.0f;

 protected:
  std::string name_val;            // Identifying name
  MatrixXf positions;                  // Vertex positions
  MatrixXf normals;                  // Vertex normals
  MatrixXf texcoords;                 // Vertex texture coordinates
  MatrixXu faces;                  // Faces
  std::shared_ptr<BSDF> bsdf = nullptr;        // BSDF of the surface
  std::shared_ptr<Light> light = nullptr;  // Associated light, if any
  DiscretePDF dpdf;
  BoundingBox3f bound_box;          // Bounding box of the mesh
};
MIN_INTERFACE(Mesh)

}  // namespace min::ray
