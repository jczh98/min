
#pragma once

#include <min-ray/vector.h>

namespace min::ray {

template <typename _PointType, typename _VectorType>
struct TRay {
  typedef _PointType PointType;
  typedef _VectorType VectorType;
  typedef typename PointType::Scalar Scalar;

  PointType o;      // Ray origin
  VectorType d;     // Ray direction
  VectorType inv_dir;  // Componentwise reciprocals of the ray direction
  Scalar mint;      // Minimum position on the ray segment
  Scalar maxt;      // Maximum position on the ray segment

  // Construct a new ray
  TRay() : mint(Epsilon),
           maxt(std::numeric_limits<Scalar>::infinity()) {}

  // Construct a new ray
  TRay(const PointType &o, const VectorType &d) : o(o), d(d), mint(Epsilon), maxt(std::numeric_limits<Scalar>::infinity()) {
    update();
  }

  // Construct a new ray
  TRay(const PointType &o, const VectorType &d,
       Scalar mint, Scalar maxt) : o(o), d(d), mint(mint), maxt(maxt) {
    update();
  }

  // Copy constructor
  TRay(const TRay &ray)
      : o(ray.o), d(ray.d), inv_dir(ray.inv_dir), mint(ray.mint), maxt(ray.maxt) {}

  // Copy a ray, but change the covered segment of the copy
  TRay(const TRay &ray, Scalar mint, Scalar maxt)
      : o(ray.o), d(ray.d), inv_dir(ray.inv_dir), mint(mint), maxt(maxt) {}

  // Update the reciprocal ray directions after changing 'd'
  void update() {
    inv_dir = d.cwiseInverse();
  }

  // Return the position of a point along the ray
  PointType operator()(Scalar t) const { return o + t * d; }

  /// Return a ray that points into the opposite direction
  TRay Reverse() const {
    TRay result;
    result.o = o;
    result.d = -d;
    result.inv_dir = -inv_dir;
    result.mint = mint;
    result.maxt = maxt;
    return result;
  }

  std::string ToString() const {
    return tfm::format(
        "Ray[\n"
        "  o = %s,\n"
        "  d = %s,\n"
        "  mint = %f,\n"
        "  maxt = %f\n"
        "]",
        o.ToString(), d.ToString(), mint, maxt);
  }
};

}  // namespace min::ray
