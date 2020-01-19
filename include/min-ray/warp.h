
#pragma once

#include <min-ray/common.h>
#include <min-ray/sampler.h>

namespace min::ray {

/// A collection of useful warping functions for importance sampling
class Warp {
 public:
  /// Dummy warping function: takes uniformly distributed points in a square and just returns them
  static Point2f SquareToUniformSquare(const Point2f &sample);

  /// Probability density of \ref squareToUniformSquare()
  static float SquareToUniformSquarePdf(const Point2f &p);

  /// Sample a 2D tent distribution
  static Point2f SquareToTent(const Point2f &sample);

  /// Probability density of \ref squareToTent()
  static float SquareToTentPdf(const Point2f &p);

  /// Uniformly sample a vector on a 2D disk with radius 1, centered around the origin
  static Point2f SquareToUniformDisk(const Point2f &sample);

  /// Probability density of \ref squareToUniformDisk()
  static float SquareToUniformDiskPdf(const Point2f &p);

  /// Uniformly sample a vector on the unit sphere with respect to solid angles
  static Vector3f SquareToUniformSphere(const Point2f &sample);

  /// Probability density of \ref squareToUniformSphere()
  static float SquareToUniformSpherePdf(const Vector3f &v);

  /// Uniformly sample a vector on the unit hemisphere around the pole (0,0,1) with respect to solid angles
  static Vector3f SquareToUniformHemisphere(const Point2f &sample);

  /// Probability density of \ref squareToUniformHemisphere()
  static float SquareToUniformHemispherePdf(const Vector3f &v);

  /// Uniformly sample a vector on the unit hemisphere around the pole (0,0,1) with respect to projected solid angles
  static Vector3f SquareToCosineHemisphere(const Point2f &sample);

  /// Probability density of \ref squareToCosineHemisphere()
  static float SquareToCosineHemispherePdf(const Vector3f &v);

  /// Warp a uniformly distributed square sample to a Beckmann distribution * cosine for the given 'alpha' parameter
  static Vector3f SquareToBeckmann(const Point2f &sample, float alpha);

  /// Probability density of \ref squareToBeckmann()
  static float SquareToBeckmannPdf(const Vector3f &m, float alpha);
};

}  // namespace min::ray
