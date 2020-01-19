#include <min-ray/frame.h>
#include <min-ray/vector.h>
#include <min-ray/warp.h>

namespace min::ray {

Point2f Warp::SquareToUniformSquare(const Point2f &sample) {
  return sample;
}

float Warp::SquareToUniformSquarePdf(const Point2f &sample) {
  return ((sample.array() >= 0).all() && (sample.array() <= 1).all()) ? 1.0f : 0.0f;
}

Point2f Warp::SquareToTent(const Point2f &sample) {
  throw NoriException("Warp::squareToTent() is not yet implemented!");
}

float Warp::SquareToTentPdf(const Point2f &p) {
  throw NoriException("Warp::squareToTentPdf() is not yet implemented!");
}

Point2f Warp::SquareToUniformDisk(const Point2f &sample) {
  throw NoriException("Warp::squareToUniformDisk() is not yet implemented!");
}

float Warp::SquareToUniformDiskPdf(const Point2f &p) {
  throw NoriException("Warp::squareToUniformDiskPdf() is not yet implemented!");
}

Vector3f Warp::SquareToUniformSphere(const Point2f &sample) {
  throw NoriException("Warp::squareToUniformSphere() is not yet implemented!");
}

float Warp::SquareToUniformSpherePdf(const Vector3f &v) {
  throw NoriException("Warp::squareToUniformSpherePdf() is not yet implemented!");
}

Vector3f Warp::SquareToUniformHemisphere(const Point2f &sample) {
  throw NoriException("Warp::squareToUniformHemisphere() is not yet implemented!");
}

float Warp::SquareToUniformHemispherePdf(const Vector3f &v) {
  throw NoriException("Warp::squareToUniformHemispherePdf() is not yet implemented!");
}

Vector3f Warp::SquareToCosineHemisphere(const Point2f &sample) {
  throw NoriException("Warp::squareToCosineHemisphere() is not yet implemented!");
}

float Warp::SquareToCosineHemispherePdf(const Vector3f &v) {
  throw NoriException("Warp::squareToCosineHemispherePdf() is not yet implemented!");
}

Vector3f Warp::SquareToBeckmann(const Point2f &sample, float alpha) {
  throw NoriException("Warp::squareToBeckmann() is not yet implemented!");
}

float Warp::SquareToBeckmannPdf(const Vector3f &m, float alpha) {
  throw NoriException("Warp::squareToBeckmannPdf() is not yet implemented!");
}

}  // namespace min::ray
