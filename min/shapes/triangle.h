#pragma once

#include <min/visual/shape.h>
#include <min/visual/intersection.h>
#include <min/visual/sampling.h>

namespace min {

struct TriangleMesh {
  int triangles_num, vertices_num;
  std::vector<int> vertex_indices;
  std::unique_ptr<Point3f[]> p;
  std::unique_ptr<Normal3f[]> n;
  std::unique_ptr<Vector3f[]> s;
  std::unique_ptr<Point2f[]> uv;
  std::vector<int> face_indices;
  TriangleMesh::TriangleMesh(
      const Transform &ObjectToWorld, int nTriangles, const int *vertexIndices,
      int nVertices, const Point3f *P, const Vector3f *S, const Normal3f *N,
      const Point2f *UV, const int *fIndices)
  : triangles_num(nTriangles),
    vertices_num(nVertices),
    vertex_indices(vertexIndices, vertexIndices + 3 * nTriangles) {

    // Transform mesh vertices to world space
    p.reset(new Point3f[nVertices]);
    for (int i = 0; i < nVertices; ++i) p[i] = ObjectToWorld.ToPoint(P[i]);

    // Copy _UV_, _N_, and _S_ vertex data, if present
    if (UV) {
      uv.reset(new Point2f[nVertices]);
      memcpy(uv.get(), UV, nVertices * sizeof(Point2f));
    }
    if (N) {
      n.reset(new Normal3f[nVertices]);
      for (int i = 0; i < nVertices; ++i) n[i] = ObjectToWorld.ToNormal(N[i]);
    }
    if (S) {
      s.reset(new Vector3f[nVertices]);
      for (int i = 0; i < nVertices; ++i) s[i] = ObjectToWorld.ToVector(S[i]);
    }
    if (fIndices)
      face_indices = std::vector<int>(fIndices, fIndices + nTriangles);
  }
};

class Triangle : public Shape {
  std::shared_ptr<TriangleMesh> mesh;
  const int *v;
  int face_index;
  void GetUVs(Point2 uv[3]) const {
    if (mesh->uv) {
      uv[0] = mesh->uv[v[0]];
      uv[1] = mesh->uv[v[1]];
      uv[2] = mesh->uv[v[2]];
    } else {
      uv[0] = Point2(0, 0);
      uv[1] = Point2(1, 0);
      uv[2] = Point2(1, 1);
      std::cout << uv[0].ToString() << std::endl;
    }
  }
 public:
  Triangle(const Transform &ObjectToWorld, const Transform &WorldToObject,
      const std::shared_ptr<TriangleMesh> &mesh, int triNumber)
      : Shape(ObjectToWorld, WorldToObject), mesh(mesh) {
    v = &mesh->vertex_indices[3 * triNumber];
    face_index = mesh->face_indices.size() ? mesh->face_indices[triNumber] : 0;
  }

  Bounds3f WorldBound() const override {
    const Point3f &p0 = mesh->p[v[0]];
    const Point3f &p1 = mesh->p[v[1]];
    const Point3f &p2 = mesh->p[v[2]];
    return Union(Bounds3f(p0, p1), p2);
  }
  Bounds3f ObjectBound() const override {
    const Point3f &p0 = mesh->p[v[0]];
    const Point3f &p1 = mesh->p[v[1]];
    const Point3f &p2 = mesh->p[v[2]];
    return Union(Bounds3f(world2object.ToPoint(p0), world2object.ToPoint(p1)), world2object.ToPoint(p2));
  }

  bool Intersect(const Ray &ray, SurfaceIntersection &isect) const override {
    const Point3f &p0 = mesh->p[v[0]];
    const Point3f &p1 = mesh->p[v[1]];
    const Point3f &p2 = mesh->p[v[2]];
    Point3f p0t = p0 - Vector3f(ray.o);
    Point3f p1t = p1 - Vector3f(ray.o);
    Point3f p2t = p2 - Vector3f(ray.o);
    auto rayd = Abs(ray.d);
    int kz = rayd.x > rayd.y ? (rayd.x > rayd.z ? 0 : 2) : (rayd.y > rayd.z ? 1 : 2);
    int kx = kz + 1;
    if (kx == 3) kx = 0;
    int ky = kx + 1;
    if (ky == 3) ky = 0;
    Vector3f d = Permute(ray.d, Vector3i(kx, ky, kz));
    p0t = Permute(p0t, Vector3i(kx, ky, kz));
    p1t = Permute(p1t, Vector3i(kx, ky, kz));
    p2t = Permute(p2t, Vector3i(kx, ky, kz));
    Float Sx = -d.x / d.z;
    Float Sy = -d.y / d.z;
    Float Sz = 1.f / d.z;
    p0t.x += Sx * p0t.z;
    p0t.y += Sy * p0t.z;
    p1t.x += Sx * p1t.z;
    p1t.y += Sy * p1t.z;
    p2t.x += Sx * p2t.z;
    p2t.y += Sy * p2t.z;
    Float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
    Float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
    Float e2 = p0t.x * p1t.y - p0t.y * p1t.x;
    if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
      return false;
    Float det = e0 + e1 + e2;
    if (det == 0) return false;
    p0t.z *= Sz;
    p1t.z *= Sz;
    p2t.z *= Sz;
    Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
    if (det < 0 && (tScaled >= 0 || tScaled < ray.tmax * det))
      return false;
    else if (det > 0 && (tScaled <= 0 || tScaled > ray.tmax * det))
      return false;
    Float invDet = 1 / det;
    Float b0 = e0 * invDet;
    Float b1 = e1 * invDet;
    Float b2 = e2 * invDet;
    Float t = tScaled * invDet;

    // Ensure that computed triangle $t$ is conservatively greater than zero

    // Compute $\delta_z$ term for triangle $t$ error bounds
    Float maxZt = MaxComp(Abs(Vector3f(p0t.z, p1t.z, p2t.z)));
    Float deltaZ = Gamma(3) * maxZt;

    // Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
    Float maxXt = MaxComp(Abs(Vector3f(p0t.x, p1t.x, p2t.x)));
    Float maxYt = MaxComp(Abs(Vector3f(p0t.y, p1t.y, p2t.y)));
    Float deltaX = Gamma(5) * (maxXt + maxZt);
    Float deltaY = Gamma(5) * (maxYt + maxZt);

    // Compute $\delta_e$ term for triangle $t$ error bounds
    Float deltaE =
        2 * (Gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

    // Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
    Float maxE = MaxComp(Abs(Vector3f(e0, e1, e2)));
    Float deltaT = 3 *
        (Gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) *
        std::abs(invDet);
    if (t <= deltaT) return false;

    Point2f uv[3];
    GetUVs(uv);

    // Compute error bounds for triangle intersection
    Float xAbsSum =
        (std::abs(b0 * p0.x) + std::abs(b1 * p1.x) + std::abs(b2 * p2.x));
    Float yAbsSum =
        (std::abs(b0 * p0.y) + std::abs(b1 * p1.y) + std::abs(b2 * p2.y));
    Float zAbsSum =
        (std::abs(b0 * p0.z) + std::abs(b1 * p1.z) + std::abs(b2 * p2.z));
    Vector3f pError = Gamma(7) * Vector3f(xAbsSum, yAbsSum, zAbsSum);

    Point3f pHit = b0 * p0 + b1 * p1 + b2 * p2;
    Point2f uvHit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];
    // Fill in _SurfaceInteraction_ from triangle hit
    isect.p = pHit;
    isect.error = pError;
    isect.wo = -ray.d;
    isect.time = ray.time;
    isect.shape = this;
    isect.face_index = face_index;
    ShadingPoint sp;
    sp.texcoords = uvHit;
    isect.sp = sp;
    ray.tmax = t;
    isect.geo_frame = Frame(Normalize(Cross((p0 - p2), (p1 - p2))));
    if (mesh->n) {
      auto ns = Normalize((b0 * mesh->n[v[0]] + b1 * mesh->n[v[1]] + b2 * mesh->n[v[2]]));
      if (ns.LengthSquared() > 0) {
        isect.shading_frame = Frame(ns);
      } else {
        isect.shading_frame = isect.geo_frame;
      }
      isect.geo_frame = Frame(Faceforward(isect.geo_frame.n, isect.shading_frame.n));
    } else {
      isect.shading_frame = isect.geo_frame;
    }
    return true;
  }

  bool IntersectP(const Ray &ray) const override {
    const Point3f &p0 = mesh->p[v[0]];
    const Point3f &p1 = mesh->p[v[1]];
    const Point3f &p2 = mesh->p[v[2]];
    Point3f p0t = p0 - Vector3f(ray.o);
    Point3f p1t = p1 - Vector3f(ray.o);
    Point3f p2t = p2 - Vector3f(ray.o);
    auto rayd = Abs(ray.d);
    int kz = rayd.x > rayd.y ? (rayd.x > rayd.z ? 0 : 2) : (rayd.y > rayd.z ? 1 : 2);
    int kx = kz + 1;
    if (kx == 3) kx = 0;
    int ky = kx + 1;
    if (ky == 3) ky = 0;
    Vector3f d = Permute(ray.d, Vector3i(kx, ky, kz));
    p0t = Permute(p0t, Vector3i(kx, ky, kz));
    p1t = Permute(p1t, Vector3i(kx, ky, kz));
    p2t = Permute(p2t, Vector3i(kx, ky, kz));
    Float Sx = -d.x / d.z;
    Float Sy = -d.y / d.z;
    Float Sz = 1.f / d.z;
    p0t.x += Sx * p0t.z;
    p0t.y += Sy * p0t.z;
    p1t.x += Sx * p1t.z;
    p1t.y += Sy * p1t.z;
    p2t.x += Sx * p2t.z;
    p2t.y += Sy * p2t.z;
    Float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
    Float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
    Float e2 = p0t.x * p1t.y - p0t.y * p1t.x;
    if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0))
      return false;
    Float det = e0 + e1 + e2;
    if (det == 0) return false;
    p0t.z *= Sz;
    p1t.z *= Sz;
    p2t.z *= Sz;
    Float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
    if (det < 0 && (tScaled >= 0 || tScaled < ray.tmax * det))
      return false;
    else if (det > 0 && (tScaled <= 0 || tScaled > ray.tmax * det))
      return false;
    // Compute barycentric coordinates and $t$ value for triangle intersection
    Float invDet = 1 / det;
    Float b0 = e0 * invDet;
    Float b1 = e1 * invDet;
    Float b2 = e2 * invDet;
    Float t = tScaled * invDet;

    // Ensure that computed triangle $t$ is conservatively greater than zero

    // Compute $\delta_z$ term for triangle $t$ error bounds
    Float maxZt = MaxComp(Abs(Vector3f(p0t.z, p1t.z, p2t.z)));
    Float deltaZ = Gamma(3) * maxZt;

    // Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
    Float maxXt = MaxComp(Abs(Vector3f(p0t.x, p1t.x, p2t.x)));
    Float maxYt = MaxComp(Abs(Vector3f(p0t.y, p1t.y, p2t.y)));
    Float deltaX = Gamma(5) * (maxXt + maxZt);
    Float deltaY = Gamma(5) * (maxYt + maxZt);

    // Compute $\delta_e$ term for triangle $t$ error bounds
    Float deltaE =
        2 * (Gamma(2) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt);

    // Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
    Float maxE = MaxComp(Abs(Vector3f(e0, e1, e2)));
    Float deltaT = 3 *
        (Gamma(3) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE) *
        std::abs(invDet);
    if (t <= deltaT) return false;
    return true;
  }
  Float Area() const override {
    const Point3f &p0 = mesh->p[v[0]];
    const Point3f &p1 = mesh->p[v[1]];
    const Point3f &p2 = mesh->p[v[2]];
    return 0.5 * Cross(p1 - p0, p2 - p0).Length();
  }
  void Sample(const Point2f &u, SurfaceSample &sample) const override {
    const Point3f &p0 = mesh->p[v[0]];
    const Point3f &p1 = mesh->p[v[1]];
    const Point3f &p2 = mesh->p[v[2]];
    Point2f b = UniformSampleTriangle(u);
    sample.p = b[0] * p0 + b[1] * p1 + (1 - b[0] - b[1]) * p2;
    sample.normal = Normalize(Cross(p1 - p0, p2 - p0));
    if (mesh->n) {
      Normal3f ns (b[0] * mesh->n[v[0]] + b[1] * mesh->n[v[1]] + (1 - b[0] - b[1]) * mesh->n[v[2]]);
      sample.normal = Dot(sample.normal, ns) < 0.0f ? sample.normal : sample.normal;
    }
    sample.pdf = 1 / Area();
    Vector3 wi = sample.p - sample.ref;
    if (wi.LengthSquared() == 0) sample.pdf = 0;
    else {
      wi = Normalize(wi);
      sample.pdf *= (sample.ref - sample.p).LengthSquared() / AbsDot(sample.normal, -wi);
      if (std::isinf(sample.pdf)) sample.pdf = 0.0f;
    }
  }
};

std::vector<std::shared_ptr<Shape>> CreateTriangleMesh(
    const Transform &object2world, const Transform &world2object,
    int nTriangles, const int *vertexIndices, int nVertices, const Point3f *p,
    const Vector3f *s, const Normal3f *n, const Point2f *uv,
    const int *faceIndices = nullptr) {
  std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>(
      object2world, nTriangles, vertexIndices, nVertices, p, s, n, uv, faceIndices);
  std::vector<std::shared_ptr<Shape>> tris;
  tris.reserve(nTriangles);
  for (int i = 0; i < nTriangles; ++i)
    tris.push_back(std::make_shared<Triangle>(object2world, world2object,
                                              mesh, i));
  return tris;
}

}

