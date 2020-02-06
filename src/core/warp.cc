#include <min-ray/frame.h>
#include <min-ray/vector.h>
#include <min-ray/warp.h>

namespace min::ray {

namespace warp {

Point2f SquareToUniformSquare(const Point2f &sample) {
  return sample;
}

float SquareToUniformSquarePdf(const Point2f &sample) {
  return ((sample.array() >= 0).all() && (sample.array() <= 1).all()) ? 1.0f : 0.0f;
}

Point2f SquareToTent(const Point2f &sample) {
  MIN_ERROR("Warp::squareToTent() is not yet implemented!");
  return {};
}

float SquareToTentPdf(const Point2f &p) {
  MIN_ERROR("Warp::squareToTentPdf() is not yet implemented!");
  return {};
}

Point2f SquareToUniformDisk(const Point2f &sample) {
  float r = sqrt(sample.x());
  float theta = 2.0f * M_PI * sample.y();
  Point2f res(r * cos(theta), r * sin(theta));
  return res;
}

float SquareToUniformDiskPdf(const Point2f &p) {
  MIN_ERROR("Warp::squareToUniformDiskPdf() is not yet implemented!");
  return {};
}

Vector3f SquareToUniformSphere(const Point2f &sample) {
  MIN_ERROR("Warp::squareToUniformSphere() is not yet implemented!");
  return {};
}

float SquareToUniformSpherePdf(const Vector3f &v) {
  MIN_ERROR("Warp::squareToUniformSpherePdf() is not yet implemented!");
  return {};
}

Vector3f SquareToUniformHemisphere(const Point2f &sample) {
  MIN_ERROR("Warp::squareToUniformHemisphere() is not yet implemented!");
  return {};
}

float SquareToUniformHemispherePdf(const Vector3f &v) {
  MIN_ERROR("Warp::squareToUniformHemispherePdf() is not yet implemented!");
  return {};
}

Vector3f SquareToCosineHemisphere(const Point2f &sample) {
  Point2f disk = SquareToUniformDisk(sample);
  return Vector3f(disk.x(), disk.y(), sqrt(1.0f - disk.squaredNorm()));
  float phi = 2 * M_PI * sample[0];
  float theta = acos(sqrt(sample[1]));
  return Vector3f(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
}

float SquareToCosineHemispherePdf(const Vector3f &v) {
  MIN_ERROR("Warp::squareToCosineHemispherePdf() is not yet implemented!");
  return {};
}

Vector3f SquareToBeckmann(const Point2f &sample, float alpha) {
  float phi = 2 * M_PI * sample[0];
  float theta = atan(alpha * sqrt(log(1 / (1 - sample[1]))));
  return Vector3f(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
}

float SquareToBeckmannPdf(const Vector3f &m, float alpha) {
  MIN_ERROR("Warp::squareToBeckmannPdf() is not yet implemented!");
  return {};
}

Vector3f SquareToUniformTriangle(const Point2f &sample) {
  float su1 = sqrtf(sample.x());
  float u = 1.f - su1, v = sample.y() * su1;
  return Vector3f(u, v, 1.f - u - v);
}

}

}  // namespace min::ray
