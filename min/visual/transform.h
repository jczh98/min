#pragma once

#include "defs.h"
#include "geometry.h"

namespace min {

class Transform {
  Matrix4f m, inv;
 public:
  static Transform Identity() {
    return Transform(Matrix4f::Identidy(), Matrix4f::Identidy());
  }

  Transform() {}
  Transform(const Matrix4f &mat) {
    m = mat;
    inv = Inverse(mat);
  }
  Transform(const Matrix4f &m, const Matrix4f &inv) : m(m), inv(inv) {}

  friend Transform Inverse(const Transform &t) {
    return Transform(t.inv, t.m);
  }

  friend Transform Transpose(const Transform &t) {
    return Transform(Transpose(t.m), Transpose(t.inv));
  }

  bool operator==(const Transform &t) const {
    return t.m == m && t.inv == inv;
  }
  bool operator!=(const Transform &t) const {
    return t.m != m || t.inv != inv;
  }

  bool operator<(const Transform &t2) const {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 4; ++j) {
        if (m[i][j] < t2.m[i][j]) return true;
        if (m[i][j] > t2.m[i][j]) return false;
      }
    return false;
  }

  Transform operator*(const Transform &t2) const {
    return Transform(m * t2.m, t2.inv * inv);
  }

  bool IsIdentity() const {
    return (m[0][0] == 1.f && m[0][1] == 0.f && m[0][2] == 0.f &&
        m[0][3] == 0.f && m[1][0] == 0.f && m[1][1] == 1.f &&
        m[1][2] == 0.f && m[1][3] == 0.f && m[2][0] == 0.f &&
        m[2][1] == 0.f && m[2][2] == 1.f && m[2][3] == 0.f &&
        m[3][0] == 0.f && m[3][1] == 0.f && m[3][2] == 0.f &&
        m[3][3] == 1.f);
  }
  const Matrix4f &GetMatrix() const { return m; }
  const Matrix4f &GetInverseMatrix() const { return inv; }

  template<typename T>
  MIN_FORCE_INLINE TPoint3<T> ToPoint(const TPoint3<T> &p) const {
    T x = p.x, y = p.y, z = p.z;
    T xp = m[0][0] * x + m[0][1] * y + m[0][2] * z + m[0][3];
    T yp = m[1][0] * x + m[1][1] * y + m[1][2] * z + m[1][3];
    T zp = m[2][0] * x + m[2][1] * y + m[2][2] * z + m[2][3];
    T wp = m[3][0] * x + m[3][1] * y + m[3][2] * z + m[3][3];
    if (wp == 1)
      return TPoint3<T>(xp, yp, zp);
    else
      return TPoint3<T>(xp, yp, zp) / wp;
  }

  template <typename T>
  MIN_FORCE_INLINE TVector3<T> ToVector(const TVector3<T> &v) const {
    T x = v.x, y = v.y, z = v.z;
    return TVector3<T>(m[0][0] * x + m[0][1] * y + m[0][2] * z,
                      m[1][0] * x + m[1][1] * y + m[1][2] * z,
                      m[2][0] * x + m[2][1] * y + m[2][2] * z);
  }

  template <typename T>
  MIN_FORCE_INLINE TNormal3<T> ToNormal(const TNormal3<T> &n) const {
    T x = n.x, y = n.y, z = n.z;
    return TNormal3<T>(inv[0][0] * x + inv[1][0] * y + inv[2][0] * z,
                      inv[0][1] * x + inv[1][1] * y + inv[2][1] * z,
                      inv[0][2] * x + inv[1][2] * y + inv[2][2] * z);
  }

};

MIN_FORCE_INLINE Transform Translate(const Vector3f &delta) {
  Matrix4f m(Vector4f(1, 0, 0, delta.x),
      Vector4f(0, 1, 0, delta.y),
      Vector4f(0, 0, 1, delta.z),
      Vector4f(0, 0, 0,1));
  Matrix4f minv(Vector4f(1, 0, 0, -delta.x),
      Vector4f(0, 1, 0, -delta.y),
      Vector4f(0, 0, 1, -delta.z),
      Vector4f(0,0, 0, 1));
  return Transform(m, minv);
}

MIN_FORCE_INLINE Transform Scale(Float x, Float y, Float z) {
  Matrix4f m(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
  Matrix4f minv(1 / x, 0, 0, 0, 0, 1 / y, 0, 0, 0, 0, 1 / z, 0, 0, 0, 0, 1);
  return Transform(m, minv);
}

MIN_FORCE_INLINE Transform RotateX(Float theta) {
  Float sinTheta = std::sin(Radians(theta));
  Float cosTheta = std::cos(Radians(theta));
  Matrix4f m(1, 0, 0, 0, 0, cosTheta, -sinTheta, 0, 0, sinTheta, cosTheta, 0,
              0, 0, 0, 1);
  return Transform(m, Transpose(m));
}

MIN_FORCE_INLINE Transform RotateY(Float theta) {
  Float sinTheta = std::sin(Radians(theta));
  Float cosTheta = std::cos(Radians(theta));
  Matrix4f m(cosTheta, 0, sinTheta, 0, 0, 1, 0, 0, -sinTheta, 0, cosTheta, 0,
              0, 0, 0, 1);
  return Transform(m, Transpose(m));
}

MIN_FORCE_INLINE Transform RotateZ(Float theta) {
  Float sinTheta = std::sin(Radians(theta));
  Float cosTheta = std::cos(Radians(theta));
  Matrix4f m(cosTheta, -sinTheta, 0, 0, sinTheta, cosTheta, 0, 0, 0, 0, 1, 0,
              0, 0, 0, 1);
  return Transform(m, Transpose(m));
}

MIN_FORCE_INLINE Transform Rotate(Float theta, const Vector3f &axis) {
  Vector3f a = Normalize(axis);
  Float sinTheta = std::sin(Radians(theta));
  Float cosTheta = std::cos(Radians(theta));
  Matrix4f m;
  // Compute rotation of first basis vector
  m[0][0] = a.x * a.x + (1 - a.x * a.x) * cosTheta;
  m[0][1] = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
  m[0][2] = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;
  m[0][3] = 0;

  // Compute rotations of second and third basis vectors
  m[1][0] = a.x * a.y * (1 - cosTheta) + a.z * sinTheta;
  m[1][1] = a.y * a.y + (1 - a.y * a.y) * cosTheta;
  m[1][2] = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;
  m[1][3] = 0;

  m[2][0] = a.x * a.z * (1 - cosTheta) - a.y * sinTheta;
  m[2][1] = a.y * a.z * (1 - cosTheta) + a.x * sinTheta;
  m[2][2] = a.z * a.z + (1 - a.z * a.z) * cosTheta;
  m[2][3] = 0;
  return Transform(m, Transpose(m));
}

MIN_FORCE_INLINE Transform LookAt(const Point3f &pos, const Point3f &look, const Vector3f &up) {
  Matrix4f cameraToWorld;
  // Initialize fourth column of viewing matrix
  cameraToWorld[0][3] = pos.x;
  cameraToWorld[1][3] = pos.y;
  cameraToWorld[2][3] = pos.z;
  cameraToWorld[3][3] = 1;

  // Initialize first three columns of viewing matrix
  Vector3f dir = Normalize(look - pos);
  if (Cross(Normalize(up), dir).Length() == 0) {
    MIN_ERROR(
        "\"up\" vector ({}, {}, {}) and viewing direction ({}, {}, {}) "
        "passed to LookAt are pointing in the same direction.  Using "
        "the identity transformation.",
        up.x, up.y, up.z, dir.x, dir.y, dir.z);
    return Transform();
  }
  Vector3f right = Normalize(Cross(Normalize(up), dir));
  Vector3f newUp = Cross(dir, right);
  cameraToWorld[0][0] = right.x;
  cameraToWorld[1][0] = right.y;
  cameraToWorld[2][0] = right.z;
  cameraToWorld[3][0] = 0.;
  cameraToWorld[0][1] = newUp.x;
  cameraToWorld[1][1] = newUp.y;
  cameraToWorld[2][1] = newUp.z;
  cameraToWorld[3][1] = 0.;
  cameraToWorld[0][2] = dir.x;
  cameraToWorld[1][2] = dir.y;
  cameraToWorld[2][2] = dir.z;
  cameraToWorld[3][2] = 0.;
  return Transform(Inverse(cameraToWorld), cameraToWorld);
}

MIN_FORCE_INLINE Transform Orthographic(Float zNear, Float zFar) {
  return Scale(1, 1, 1 / (zFar - zNear)) * Translate(Vector3f(0, 0, -zNear));
}

MIN_FORCE_INLINE Transform Perspective(Float fov, Float znear, Float zfar) {
  // Perform projective divide for perspective projection
  Matrix4f persp(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, zfar / (zfar - znear), -zfar * znear / (zfar - znear),
                  0, 0, 1, 0);

  // Scale canonical perspective view to specified field of view
  Float invTanAng = 1 / std::tan(Radians(fov) / 2);
  return Scale(invTanAng, invTanAng, 1) * Transform(persp);
}


}

namespace nlohmann {

template <>
struct adl_serializer<Transform> {
  static void from_json(const json& j, Transform &v) {
    auto transform = Transform::Identity();
    if (j.contains("scale")) {
      auto vec = j.at("scale").get<Vector3f>();
      transform = transform * Scale(vec.x, vec.y, vec.z);
    }
    if (j.contains("lookat")) {
      auto target = j.at("lookat").at("target").get<Vector3f>();
      auto origin = j.at("lookat").at("origin").get<Vector3f>();
      auto up = j.at("lookat").at("up").get<Vector3f>();
      transform = transform * LookAt(origin, target, up);
    }
    v = Inverse(transform);
  }
};

}


