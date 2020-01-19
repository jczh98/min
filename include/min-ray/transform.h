#pragma once

#include <min-ray/common.h>
#include <min-ray/ray.h>

namespace min::ray {

/**
 * \brief Homogeneous coordinate transformation
 *
 * This class stores a general homogeneous coordinate tranformation, such as
 * rotation, translation, uniform or non-uniform scaling, and perspective
 * transformations. The inverse of this transformation is also recorded
 * here, since it is required when transforming normal vectors.
 */
struct Transform {
 public:
  /// Create the identity transform
  Transform() : transform(Eigen::Matrix4f::Identity()),
                inverse(Eigen::Matrix4f::Identity()) {}

  /// Create a new transform instance for the given matrix
  Transform(const Eigen::Matrix4f &trafo);

  /// Create a new transform instance for the given matrix and its inverse
  Transform(const Eigen::Matrix4f &trafo, const Eigen::Matrix4f &inv)
      : transform(trafo), inverse(inv) {}

  /// Return the underlying matrix
  const Eigen::Matrix4f &GetMatrix() const {
    return transform;
  }

  /// Return the inverse of the underlying matrix
  const Eigen::Matrix4f &GetInverseMatrix() const {
    return inverse;
  }

  /// Return the inverse transformation
  Transform Inverse() const {
    return Transform(inverse, transform);
  }

  /// Concatenate with another transform
  Transform operator*(const Transform &t) const;

  /// Apply the homogeneous transformation to a 3D vector
  Vector3f operator*(const Vector3f &v) const {
    return transform.topLeftCorner<3, 3>() * v;
  }

  /// Apply the homogeneous transformation to a 3D normal
  Normal3f operator*(const Normal3f &n) const {
    return inverse.topLeftCorner<3, 3>().transpose() * n;
  }

  /// Transform a point by an arbitrary matrix in homogeneous coordinates
  Point3f operator*(const Point3f &p) const {
    Vector4f result = transform * Vector4f(p[0], p[1], p[2], 1.0f);
    return result.head<3>() / result.w();
  }

  /// Apply the homogeneous transformation to a ray
  Ray3f operator*(const Ray3f &r) const {
    return Ray3f(
        operator*(r.o),
        operator*(r.d),
        r.mint, r.maxt);
  }

  /// Return a string representation
  std::string ToString() const;

 private:
  Eigen::Matrix4f transform;
  Eigen::Matrix4f inverse;
};

}  // namespace min::ray
