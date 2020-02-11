#pragma once

#include <min/common/util.h>
#include <min/common/interface.h>
#include <min/common/json.h>
#include <min/math/math.h>

namespace min {

using Point3f = Vector3f;
using Point3 = Vector3;
using Point3d = Vector3d;
using Point3i = Vector3i;
using Point2i = Vector2i;
using Point2f = Vector2f;
using Point2 = Vector2;

using Normal3f = Vector3f;
using Norma3 = Vector3;
using Norma3d = Vector3d;
using Norma3i = Vector3i;
using Norma2i = Vector2i;
using Norma2f = Vector2f;

static constexpr Float kMaxFloat = std::numeric_limits<Float>::max();
static constexpr Float kInfinity = std::numeric_limits<Float>::infinity();
static constexpr Float kRayBias = 0.0001f;
static constexpr Float kPi = 3.14159265358979323846;
static constexpr Float kInvPi = 0.31830988618379067154;
static constexpr Float kInv2Pi = 0.15915494309189533577;
static constexpr Float kInv4Pi = 0.07957747154594766788;
static constexpr Float kPiOver2 = 1.57079632679489661923;
static constexpr Float kPiOver4 = 0.78539816339744830961;
static constexpr Float kSqrt2 = 1.41421356237309504880;

// Forward declaration
class Film;
class FilmTile;
class Intersection;
class SurfaceIntersection;
class Scene;
}


