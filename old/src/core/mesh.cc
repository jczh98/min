
#include <min-ray/bbox.h>
#include <min-ray/bsdf.h>
#include <min-ray/light.h>
#include <min-ray/mesh.h>
#include <min-ray/warp.h>
#include <Eigen/Geometry>
#include <min-ray/json.h>

namespace min::ray {

void Mesh::initialize(const Json &json) {
  if (!bsdf) {
    auto bsdf_json = rjson::AtOrEmpty(json, "bsdf");
    bsdf = CreateInstance<BSDF>(rjson::GetOrDefault<std::string>(bsdf_json, "type", "diffuse"),
        rjson::GetProps(bsdf_json));
  }
  dpdf.Reserve(GetTriangleCount());
  for(uint32_t i = 0 ; i < GetTriangleCount() ; ++i) {
    dpdf.Append(surface_area(i));
  }
  dpdf.Normalize();
  if (json.contains("light")) {
    auto light_ = CreateInstance<Light>(json["light"]["type"], json["light"]["props"]);
    if (this->light)
      MIN_ERROR("Mesh: tried to register multiple Emitter instances!");
    this->light = light_;
    this->light->SetMesh(this);
  }
}


void Mesh::Sample(const Point2f &sample, SurfaceSample &surface_sample) const {
  Point2f s = sample;
  size_t idT = dpdf.SampleReuse(s.x());

  Vector3f bc = warp::SquareToUniformTriangle(s);

  surface_sample.p = GetInterpolatedVertex(idT,bc);
  if (normals.size() > 0) {
    surface_sample.n = GetInterpolatedNormal(idT, bc);
  }
  else {
    Point3f p0 = positions.col(faces(0, idT));
    Point3f p1 = positions.col(faces(1, idT));
    Point3f p2 = positions.col(faces(2, idT));
    Normal3f n = (p1-p0).cross(p2-p0).normalized();
    surface_sample.n = n;
  }
  surface_sample.pdf = 1.0f / dpdf.normalized();
}

float Mesh::Pdf(const SurfaceSample &surface_sample) const {
    return dpdf.normalization();
}

Point3f Mesh::GetInterpolatedVertex(uint32_t index, const Vector3f &bc) const {
  return (bc.x() * positions.col(faces(0, index)) +
      bc.y() * positions.col(faces(1, index)) +
      bc.z() * positions.col(faces(2, index)));
}

Normal3f Mesh::GetInterpolatedNormal(uint32_t index, const Vector3f &bc) const {
  return (bc.x() * normals.col(faces(0, index)) +
      bc.y() * normals.col(faces(1, index)) +
      bc.z() * normals.col(faces(2, index))).normalized();
}

float Mesh::surface_area(uint32_t index) const {
  uint32_t i0 = faces(0, index), i1 = faces(1, index), i2 = faces(2, index);

  const Point3f p0 = positions.col(i0), p1 = positions.col(i1), p2 = positions.col(i2);

  return 0.5f * Vector3f((p1 - p0).cross(p2 - p0)).norm();
}

bool Mesh::Intersect(uint32_t index, const Ray3f &ray, float &u, float &v, float &t) const {
  uint32_t i0 = faces(0, index), i1 = faces(1, index), i2 = faces(2, index);
  const Point3f p0 = positions.col(i0), p1 = positions.col(i1), p2 = positions.col(i2);

  // Find vectors for two edges sharing v[0]
  Vector3f edge1 = p1 - p0, edge2 = p2 - p0;

  // Begin calculating determinant - also used to calculate U parameter
  Vector3f pvec = ray.d.cross(edge2);

  // If determinant is near zero, ray lies in plane of triangle
  float det = edge1.dot(pvec);

  if (det > -1e-8f && det < 1e-8f)
    return false;
  float inv_det = 1.0f / det;

  // Calculate distance from v[0] to ray origin
  Vector3f tvec = ray.o - p0;

  // Calculate U parameter and test bounds
  u = tvec.dot(pvec) * inv_det;
  if (u < 0.0 || u > 1.0)
    return false;

  // Prepare to test V parameter
  Vector3f qvec = tvec.cross(edge1);

  // Calculate V parameter and test bounds
  v = ray.d.dot(qvec) * inv_det;
  if (v < 0.0 || u + v > 1.0)
    return false;

  // Ray intersects triangle -> compute t
  t = edge2.dot(qvec) * inv_det;

  return t >= ray.mint && t <= ray.maxt;
}

BoundingBox3f Mesh::GetBoundingBox(uint32_t index) const {
  BoundingBox3f result(positions.col(faces(0, index)));
  result.ExpandBy(positions.col(faces(1, index)));
  result.ExpandBy(positions.col(faces(2, index)));
  return result;
}

Point3f Mesh::centroid(uint32_t index) const {
  return (1.0f / 3.0f) *
         (positions.col(faces(0, index)) +
          positions.col(faces(1, index)) +
          positions.col(faces(2, index)));
}

}  // namespace min::ray