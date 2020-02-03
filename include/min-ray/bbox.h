#pragma once

#include <min-ray/ray.h>

namespace min::ray {

template <typename _PointType>
struct TBoundingBox {
  enum {
    Dimension = _PointType::Dimension
  };

  typedef _PointType PointType;
  typedef typename PointType::Scalar Scalar;
  typedef typename PointType::VectorType VectorType;

  TBoundingBox() {
    Reset();
  }

  TBoundingBox(const PointType &p)
      : min(p), max(p) {}

  TBoundingBox(const PointType &min, const PointType &max)
      : min(min), max(max) {
  }

  bool operator==(const TBoundingBox &bbox) const {
    return min == bbox.min && max == bbox.max;
  }

  bool operator!=(const TBoundingBox &bbox) const {
    return min != bbox.min || max != bbox.max;
  }

  Scalar volume() const {
    return (max - min).prod();
  }

  float surface_area() const {
    VectorType d = max - min;
    float result = 0.0f;
    for (int i = 0; i < Dimension; ++i) {
      float term = 1.0f;
      for (int j = 0; j < Dimension; ++j) {
        if (i == j)
          continue;
        term *= d[j];
      }
      result += term;
    }
    return 2.0f * result;
  }

  PointType centeriod() const {
    return (max + min) * (Scalar)0.5f;
  }

  // Check whether a point lies on or inside the bounding box
  // \param strict Set this parameter to \c true if the bounding
  // box boundary should be excluded in the test
  bool Contains(const PointType &p, bool strict = false) const {
    if (strict) {
      return (p.array() > min.array()).all() && (p.array() < max.array()).all();
    } else {
      return (p.array() >= min.array()).all() && (p.array() <= max.array()).all();
    }
  }

  // Check whether a specified bounding box lies on or within the current bounding box
  bool Contains(const TBoundingBox &bbox, bool strict = false) const {
    if (strict) {
      return (bbox.min.array() > min.array()).all() && (bbox.max.array() < max.array()).all();
    } else {
      return (bbox.min.array() >= min.array()).all() && (bbox.max.array() <= max.array()).all();
    }
  }

  // Check two axis-aligned bounding boxes for possible overlap.
  bool Overlaps(const TBoundingBox &bbox, bool strict = false) const {
    if (strict) {
      return (bbox.min.array() < max.array()).all() && (bbox.max.array() > min.array()).all();
    } else {
      return (bbox.min.array() <= max.array()).all() && (bbox.max.array() >= min.array()).all();
    }
  }

  // Calculate the smallest squared distance between the axis-aligned bounding box and the point.
  Scalar SquaredDistanceTo(const PointType &p) const {
    Scalar result = 0;

    for (int i = 0; i < Dimension; ++i) {
      Scalar value = 0;
      if (p[i] < min[i])
        value = min[i] - p[i];
      else if (p[i] > max[i])
        value = p[i] - max[i];
      result += value * value;
    }

    return result;
  }

  // Calculate the smallest distance between the axis-aligned bounding box and the point
  Scalar DistanceTo(const PointType &p) const {
    return std::sqrt(SquaredDistanceTo(p));
  }

  // Calculate the smallest square distance between the axis-aligned bounding box and bbox.
  Scalar SquaredDistanceTo(const TBoundingBox &bbox) const {
    Scalar result = 0;

    for (int i = 0; i < Dimension; ++i) {
      Scalar value = 0;
      if (bbox.max[i] < min[i])
        value = min[i] - bbox.max[i];
      else if (bbox.min[i] > max[i])
        value = bbox.min[i] - max[i];
      result += value * value;
    }

    return result;
  }

  // Calculate the smallest distance between the axis-aligned bounding box and bbox.
  Scalar DistanceTo(const TBoundingBox &bbox) const {
    return std::sqrt(SquaredDistanceTo(bbox));
  }

  // Check whether this is a valid bounding box.
  bool valid() const {
    return (max.array() >= min.array()).all();
  }

  // Check whether this bounding box has collapsed to a single point
  bool IsPoint() const {
    return (max.array() == min.array()).all();
  }

  // Check whether this bounding box has any associated volume
  bool HasVolume() const {
    return (max.array() > min.array()).all();
  }

  // Return the dimension index with the largest associated side length
  int GetMajorAxis() const {
    VectorType d = max - min;
    int largest = 0;
    for (int i = 1; i < Dimension; ++i)
      if (d[i] > d[largest])
        largest = i;
    return largest;
  }

  // Return the dimension index with the shortest associated side length
  int GetMinorAxis() const {
    VectorType d = max - min;
    int shortest = 0;
    for (int i = 1; i < Dimension; ++i)
      if (d[i] < d[shortest])
        shortest = i;
    return shortest;
  }

  // Calculate the bounding box extents.
  VectorType GetExtents() const {
    return max - min;
  }

  // Clip to another bounding box
  void Clip(const TBoundingBox &bbox) {
    min = min.cwiseMax(bbox.min);
    max = max.cwiseMin(bbox.max);
  }

  // Mark the bounding box as invalid.
  void Reset() {
    min.setConstant(std::numeric_limits<Scalar>::infinity());
    max.setConstant(-std::numeric_limits<Scalar>::infinity());
  }

  // Expand the bounding box to contain another point
  void ExpandBy(const PointType &p) {
    min = min.cwiseMin(p);
    max = max.cwiseMax(p);
  }

  // Expand the bounding box to contain another bounding box
  void ExpandBy(const TBoundingBox &bbox) {
    min = min.cwiseMin(bbox.min);
    max = max.cwiseMax(bbox.max);
  }

  // Merge two bounding boxes
  static TBoundingBox Merge(const TBoundingBox &bbox1, const TBoundingBox &bbox2) {
    return TBoundingBox(
        bbox1.min.cwiseMin(bbox2.min),
        bbox1.max.cwiseMax(bbox2.max));
  }

  // Return the index of the largest axis
  int GetLargestAxis() const {
    VectorType extents = max - min;

    if (extents[0] >= extents[1] && extents[0] >= extents[2])
      return 0;
    else if (extents[1] >= extents[0] && extents[1] >= extents[2])
      return 1;
    else
      return 2;
  }

  // Return the position of a bounding box corner
  PointType corner(int index) const {
    PointType result;
    for (int i = 0; i < Dimension; ++i)
      result[i] = (index & (1 << i)) ? max[i] : min[i];
    return result;
  }

  // Return a string representation of the bounding box
  std::string ToString() const {
    if (!valid())
      return "BoundingBox[invalid]";
    else
      return tfm::format("BoundingBox[min=%s, max=%s]", min.ToString(), max.ToString());
  }

  // Check if a ray intersects a bounding box
  bool Intersect(const Ray3f &ray) const {
    float nearT = -std::numeric_limits<float>::infinity();
    float farT = std::numeric_limits<float>::infinity();

    for (int i = 0; i < 3; i++) {
      float origin = ray.o[i];
      float minVal = min[i], maxVal = max[i];

      if (ray.d[i] == 0) {
        if (origin < minVal || origin > maxVal)
          return false;
      } else {
        float t1 = (minVal - origin) * ray.inv_dir[i];
        float t2 = (maxVal - origin) * ray.inv_dir[i];

        if (t1 > t2)
          std::swap(t1, t2);

        nearT = std::max(t1, nearT);
        farT = std::min(t2, farT);

        if (!(nearT <= farT))
          return false;
      }
    }

    return ray.mint <= farT && nearT <= ray.maxt;
  }

  // Return the overlapping region of the bounding box and an unbounded ray
  bool Intersect(const Ray3f &ray, float &nearT, float &farT) const {
    nearT = -std::numeric_limits<float>::infinity();
    farT = std::numeric_limits<float>::infinity();

    for (int i = 0; i < 3; i++) {
      float origin = ray.o[i];
      float minVal = min[i], maxVal = max[i];

      if (ray.d[i] == 0) {
        if (origin < minVal || origin > maxVal)
          return false;
      } else {
        float t1 = (minVal - origin) * ray.inv_dir[i];
        float t2 = (maxVal - origin) * ray.inv_dir[i];

        if (t1 > t2)
          std::swap(t1, t2);

        nearT = std::max(t1, nearT);
        farT = std::min(t2, farT);

        if (!(nearT <= farT))
          return false;
      }
    }

    return true;
  }

  PointType min;  // Component-wise minimum
  PointType max;  // Component-wise maximum
};

}  // namespace min::ray
