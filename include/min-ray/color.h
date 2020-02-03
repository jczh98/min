#pragma once

#include <min-ray/common.h>

namespace min::ray {

struct Color3f : public Eigen::Array3f {
 public:
  typedef Eigen::Array3f Base;

  Color3f(float value = 0.f) : Base(value, value, value) {}

  Color3f(float r, float g, float b) : Base(r, g, b) {}

  template <typename Derived>
  Color3f(const Eigen::ArrayBase<Derived> &p)
      : Base(p) {}

  template <typename Derived>
  Color3f &operator=(const Eigen::ArrayBase<Derived> &p) {
    this->Base::operator=(p);
    return *this;
  }

  float &r() { return x(); }

  const float &r() const { return x(); }

  float &g() { return y(); }

  const float &g() const { return y(); }

  float &b() { return z(); }

  const float &b() const { return z(); }

  Color3f Clamp() const { return Color3f(std::max<float>(r(), 0.0f),
                                         std::max<float>(g(), 0.0f), std::max<float>(b(), 0.0f)); }

  bool Valid() const;

  Color3f ToLinearRGB() const;

  Color3f ToSRGB() const;

  float luminance() const;

  std::string ToString() const {
    return fmt::format("[{}, {}, {}]", coeff(0), coeff(1), coeff(2));
  }
};

struct Color4f : public Eigen::Array4f {
 public:
  typedef Eigen::Array4f Base;

  Color4f() : Base(0.0f, 0.0f, 0.0f, 0.0f) {}

  Color4f(const Color3f &c) : Base(c.r(), c.g(), c.b(), 1.0f) {}

  Color4f(float r, float g, float b, float w) : Base(r, g, b, w) {}

  template <typename Derived>
  Color4f(const Eigen::ArrayBase<Derived> &p)
      : Base(p) {}

  template <typename Derived>
  Color4f &operator=(const Eigen::ArrayBase<Derived> &p) {
    this->Base::operator=(p);
    return *this;
  }

  Color3f DivideByFilterWeight() const {
    if (w() != 0)
      return head<3>() / w();
    else
      return Color3f(0.0f);
  }

  std::string ToString() const {
    return fmt::format("[{}, {}, {}, {}]", coeff(0), coeff(1), coeff(2), coeff(3));
  }
};

}  // namespace min::ray
