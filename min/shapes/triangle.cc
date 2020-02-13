#include <min/visual/shape.h>

namespace min {

struct TriangleMesh {
  int triangles_num, vertices_num;
  std::vector<int> vertex_indices;
  std::unique_ptr<Point3f[]> p;
  std::unique_ptr<Normal3f[]> n;
  std::unique_ptr<Vector3f[]> s;
  std::unique_ptr<Point2f[]> uv;
  std::vector<int> face_indices;
};

class Triangle : public Shape {
  std::shared_ptr<TriangleMesh> mesh;
  const int *v;
  int face_index;
 public:
  Bounds3f WorldBound() const override {
    const Point3f &p0 = mesh->p[v[0]];
    const Point3f &p1 = mesh->p[v[1]];
    const Point3f &p2 = mesh->p[v[2]];
    return Union(Bounds3f(p0, p1), p2);
  }
  Bounds3f ObjectBound() const override {
    return Bounds3f();
  }
  bool Intersect(const Ray &ray, SurfaceIntersection &isect) const override {
    return false;
  }
  Float Area() const override {
    const Point3f &p0 = mesh->p[v[0]];
    const Point3f &p1 = mesh->p[v[1]];
    const Point3f &p2 = mesh->p[v[2]];
    return 0.5 * Cross(p1 - p0, p2 - p0).Length();
  }
  void Sample(const Point2f &u, SurfaceSample &sample) const override {

  }
};

}

