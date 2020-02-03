#include <min-ray/frame.h>
#include <min-ray/vector.h>
#include <min-ray/sampling.h>

namespace min::ray {

Point2f Warp::SquareToUniformSquare(const Point2f &sample) {
  return sample;
}

float Warp::SquareToUniformSquarePdf(const Point2f &sample) {
  return ((sample.array() >= 0).all() && (sample.array() <= 1).all()) ? 1.0f : 0.0f;
}

Point2f Warp::SquareToTent(const Point2f &sample) {
  MIN_ERROR("Warp::squareToTent() is not yet implemented!");
  return {};
}

float Warp::SquareToTentPdf(const Point2f &p) {
  MIN_ERROR("Warp::squareToTentPdf() is not yet implemented!");
  return {};
}

Point2f Warp::SquareToUniformDisk(const Point2f &sample) {
  MIN_ERROR("Warp::squareToUniformDisk() is not yet implemented!");
  return {};
}

float Warp::SquareToUniformDiskPdf(const Point2f &p) {
  MIN_ERROR("Warp::squareToUniformDiskPdf() is not yet implemented!");
  return {};
}

Vector3f Warp::SquareToUniformSphere(const Point2f &sample) {
  MIN_ERROR("Warp::squareToUniformSphere() is not yet implemented!");
  return {};
}

float Warp::SquareToUniformSpherePdf(const Vector3f &v) {
  MIN_ERROR("Warp::squareToUniformSpherePdf() is not yet implemented!");
  return {};
}

Vector3f Warp::SquareToUniformHemisphere(const Point2f &sample) {
  MIN_ERROR("Warp::squareToUniformHemisphere() is not yet implemented!");
  return {};
}

float Warp::SquareToUniformHemispherePdf(const Vector3f &v) {
  MIN_ERROR("Warp::squareToUniformHemispherePdf() is not yet implemented!");
  return {};
}

Vector3f Warp::SquareToCosineHemisphere(const Point2f &sample) {
  float phi = 2 * M_PI * sample[0];
  float theta = acos(sqrt(sample[1]));
  return Vector3f(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
}

float Warp::SquareToCosineHemispherePdf(const Vector3f &v) {
  MIN_ERROR("Warp::squareToCosineHemispherePdf() is not yet implemented!");
  return {};
}

Vector3f Warp::SquareToBeckmann(const Point2f &sample, float alpha) {
  MIN_ERROR("Warp::squareToBeckmann() is not yet implemented!");
  return {};
}

float Warp::SquareToBeckmannPdf(const Vector3f &m, float alpha) {
  MIN_ERROR("Warp::squareToBeckmannPdf() is not yet implemented!");
  return {};
}

Vector3f Warp::SquareToUniformTriangle(const Point2f &sample) {
  float su1 = sqrtf(sample.x());
  float u = 1.f - su1, v = sample.y() * su1;
  return Vector3f(u,v,1.f-u-v);
}


}  // namespace min::ray
