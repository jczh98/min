#pragma once

#include <min-ray/common.h>
#include <min-ray/ray.h>

namespace min::ray {

struct Transform {
 public:

  Transform() : transform(Eigen::Matrix4f::Identity()),
                inverse(Eigen::Matrix4f::Identity()) {}

  Transform(const Eigen::Matrix4f &trafo);

  Transform(const Eigen::Matrix4f &trafo, const Eigen::Matrix4f &inv)
      : transform(trafo), inverse(inv) {}

  const Eigen::Matrix4f &GetMatrix() const {
    return transform;
  }

  const Eigen::Matrix4f &GetInverseMatrix() const {
    return inverse;
  }

  Transform Inverse() const {
    return Transform(inverse, transform);
  }

  Transform operator*(const Transform &t) const;

  Vector3f operator*(const Vector3f &v) const {
    return transform.topLeftCorner<3, 3>() * v;
  }

  Normal3f operator*(const Normal3f &n) const {
    return inverse.topLeftCorner<3, 3>().transpose() * n;
  }

  Point3f operator*(const Point3f &p) const {
    Vector4f result = transform * Vector4f(p[0], p[1], p[2], 1.0f);
    return result.head<3>() / result.w();
  }

  Ray3f operator*(const Ray3f &r) const {
    return Ray3f(
        operator*(r.o),
        operator*(r.d),
        r.mint, r.maxt);
  }

  std::string ToString() const;

 private:
  Eigen::Matrix4f transform;
  Eigen::Matrix4f inverse;
};

}  // namespace min::ray
