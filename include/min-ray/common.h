#pragma once

#if defined(_MSC_VER)
// Disable some warnings on MSVC++
#pragma warning(disable : 4127 4702 4100 4515 4800 4146 4512)
#define WIN32_LEAN_AND_MEAN // Don't ever include MFC on Windows
#define NOMINMAX            // Don't override min/max
#endif

#include <stdint.h>
#include <Eigen/Core>
#include <algorithm>
#include <iostream>
#include <fmt/format.h>
#include <vector>
#include "resolver.h"
#include "interface.h"

#if defined(__APPLE__)
#define PLATFORM_MACOS
#elif defined(__linux__)
#define PLATFORM_LINUX
#elif defined(WIN32)
#define PLATFORM_WINDOWS
#endif

// "Ray epsilon": relative error threshold for ray intersection computations
#define Epsilon 1e-4f

// A few useful constants
#undef M_PI

#define M_PI 3.14159265358979323846f
#define INV_PI 0.31830988618379067154f
#define INV_TWOPI 0.15915494309189533577f
#define INV_FOURPI 0.07957747154594766788f
#define SQRT_TWO 1.41421356237309504880f
#define INV_SQRT_TWO 0.70710678118654752440f

namespace min::ray {

// Forward declarations
template <typename Scalar, int Dimension>
struct TVector;
template <typename Scalar, int Dimension>
struct TPoint;
template <typename Point, typename Vector>
struct TRay;
template <typename Point>
struct TBoundingBox;

typedef TVector<float, 1> Vector1f;
typedef TVector<float, 2> Vector2f;
typedef TVector<float, 3> Vector3f;
typedef TVector<float, 4> Vector4f;
typedef TVector<double, 1> Vector1d;
typedef TVector<double, 2> Vector2d;
typedef TVector<double, 3> Vector3d;
typedef TVector<double, 4> Vector4d;
typedef TVector<int, 1> Vector1i;
typedef TVector<int, 2> Vector2i;
typedef TVector<int, 3> Vector3i;
typedef TVector<int, 4> Vector4i;
typedef TPoint<float, 1> Point1f;
typedef TPoint<float, 2> Point2f;
typedef TPoint<float, 3> Point3f;
typedef TPoint<float, 4> Point4f;
typedef TPoint<double, 1> Point1d;
typedef TPoint<double, 2> Point2d;
typedef TPoint<double, 3> Point3d;
typedef TPoint<double, 4> Point4d;
typedef TPoint<int, 1> Point1i;
typedef TPoint<int, 2> Point2i;
typedef TPoint<int, 3> Point3i;
typedef TPoint<int, 4> Point4i;
typedef TBoundingBox<Point1f> BoundingBox1f;
typedef TBoundingBox<Point2f> BoundingBox2f;
typedef TBoundingBox<Point3f> BoundingBox3f;
typedef TBoundingBox<Point4f> BoundingBox4f;
typedef TBoundingBox<Point1d> BoundingBox1d;
typedef TBoundingBox<Point2d> BoundingBox2d;
typedef TBoundingBox<Point3d> BoundingBox3d;
typedef TBoundingBox<Point4d> BoundingBox4d;
typedef TBoundingBox<Point1i> BoundingBox1i;
typedef TBoundingBox<Point2i> BoundingBox2i;
typedef TBoundingBox<Point3i> BoundingBox3i;
typedef TBoundingBox<Point4i> BoundingBox4i;
typedef TRay<Point2f, Vector2f> Ray2f;
typedef TRay<Point3f, Vector3f> Ray3f;

// Some more forward declarations
class BSDF;
class Bitmap;
class BlockGenerator;
class Camera;
class ImageBlock;
class Integrator;
class KDTree;
class Light;
class Mesh;
class PhaseFunction;
class ReconstructionFilter;
class Sampler;
class Scene;

// Import cout, cerr, endl for debugging purposes
using std::cerr;
using std::cout;
using std::endl;

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> MatrixXf;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic> MatrixXu;

// Return the number of cores (real and virtual)
extern int getCoreCount();

// Indent a string by the specified number of spaces
extern std::string Indent(const std::string &string, int amount = 2);

// Convert a string to lower case
extern std::string ToLower(const std::string &value);

// Convert a string into an boolean value
extern bool ToBool(const std::string &str);

// Convert a string into a signed integer value
extern int ToInt(const std::string &str);

// Convert a string into an unsigned integer value
extern unsigned int ToUInt(const std::string &str);

// Convert a string into a floating point value
extern float ToFloat(const std::string &str);

// Convert a string into a 3D vector
extern Eigen::Vector3f ToVector3f(const std::string &str);

// Tokenize a string into a list by splitting at 'delim'
extern std::vector<std::string> Tokenize(const std::string &s, const std::string &delim = ", ", bool includeEmpty = false);

// Check if a string ends with another string
extern bool EndsWith(const std::string &value, const std::string &ending);

// Convert a time value in milliseconds into a human-readable string
extern std::string TimeString(double time, bool precise = false);

// Convert a memory amount in bytes into a human-readable string
extern std::string MemString(size_t size, bool precise = false);

// Measures associated with probability distributions
enum EMeasure {
  EUnknownMeasure = 0,
  ESolidAngle,
  EDiscrete
};

// Convert radians to degrees
inline float RadToDeg(float value) { return value * (180.0f / M_PI); }

// Convert degrees to radians
inline float DegToRad(float value) { return value * (M_PI / 180.0f); }

#if !defined(_GNU_SOURCE)
// Emulate sincosf using sinf() and cosf()
inline void sincosf(float theta, float *_sin, float *_cos) {
  *_sin = sinf(theta);
  *_cos = cosf(theta);
}
#endif

// Simple floating point clamping function
inline float Clamp(float value, float min, float max) {
  if (value < min)
    return min;
  else if (value > max)
    return max;
  else
    return value;
}

// Simple integer clamping function
inline int Clamp(int value, int min, int max) {
  if (value < min)
    return min;
  else if (value > max)
    return max;
  else
    return value;
}

// Linearly interpolate between two values
inline float Lerp(float t, float v1, float v2) {
  return ((float)1 - t) * v1 + t * v2;
}

// Always-positive modulo operation
inline int Mod(int a, int b) {
  int r = a % b;
  return (r < 0) ? r + b : r;
}

// Compute a direction for the given coordinates in spherical coordinates
extern Vector3f SphericalDirection(float theta, float phi);

// Compute a direction for the given coordinates in spherical coordinates
extern Point2f SphericalCoordinates(const Vector3f &dir);

extern float Fresnel(float cosThetaI, float extIOR, float intIOR);


extern Resolver *GetFileResolver();

}  // namespace min::ray
