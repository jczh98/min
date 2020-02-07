
#pragma once

#include <min-ray/common.h>
#include <min-ray/sampler.h>

namespace min::ray {

namespace warp {

// Dummy warping function: takes uniformly distributed points in a square and just returns them
extern Point2f SquareToUniformSquare(const Point2f &sample);

// Probability density of \ref squareToUniformSquare()
extern float SquareToUniformSquarePdf(const Point2f &p);

// Sample a 2D tent distribution
extern Point2f SquareToTent(const Point2f &sample);

// Probability density of \ref squareToTent()
extern float SquareToTentPdf(const Point2f &p);

// Uniformly sample a vector on a 2D disk with radius 1, centered around the origin
extern Point2f SquareToUniformDisk(const Point2f &sample);

// Probability density of \ref squareToUniformDisk()
extern float SquareToUniformDiskPdf(const Point2f &p);

// Uniformly sample a vector on the unit sphere with respect to solid angles
extern Vector3f SquareToUniformSphere(const Point2f &sample);

// Probability density of \ref squareToUniformSphere()
extern float SquareToUniformSpherePdf(const Vector3f &v);

// Uniformly sample a vector on the unit hemisphere around the pole (0,0,1) with respect to solid angles
extern Vector3f SquareToUniformHemisphere(const Point2f &sample);

// Probability density of \ref squareToUniformHemisphere()
extern float SquareToUniformHemispherePdf(const Vector3f &v);

// Uniformly sample a vector on the unit hemisphere around the pole (0,0,1) with respect to projected solid angles
extern Vector3f SquareToCosineHemisphere(const Point2f &sample);

// Probability density of \ref squareToCosineHemisphere()
extern float SquareToCosineHemispherePdf(const Vector3f &v);

// Warp a uniformly distributed square sample to a Beckmann distribution * cosine for the given 'alpha' parameter
extern Vector3f SquareToBeckmann(const Point2f &sample, float alpha);

// Probability density of \ref squareToBeckmann()
extern float SquareToBeckmannPdf(const Vector3f &m, float alpha);

// Uniformly sample a vector on the unit sphere with respect to solid angles
extern Vector3f SquareToUniformTriangle(const Point2f &sample);

};

}  // namespace min::ray
