#pragma once

#include <min/visual/defs.h>
#include <min/visual/frame.h>

namespace min {

class MicrofacetDistribution {
 public:
  enum Type {
    kBeckmann = 0,
    kGGX = 1
  };

  Type type;
  Float alpha_x, alpha_y;
  MicrofacetDistribution(Type type, Float alpha) : type(type), alpha_x(alpha), alpha_y(alpha) {}
  MicrofacetDistribution(Type type, Float alpha_x, Float alpha_y) : type(type), alpha_x(alpha_x), alpha_y(alpha_y) {}

  Float Evaluate(const Vector3 &wh) const {
    Float result;
    switch (type) {
      case kBeckmann: {
        Float tan2theta = Frame::Sin2Theta(wh) / Frame::Cos2Theta(wh);
        if (std::isinf(tan2theta)) {
          result = 0.0f;
          break;
        }
        Float cos4theta = Frame::Cos2Theta(wh) * Frame::Cos2Theta(wh);
        result =
            std::exp(-tan2theta * (Frame::Cos2Phi(wh) / (alpha_x * alpha_x) + Frame::Sin2Phi(wh) / (alpha_y * alpha_y)))
                / (kPi * alpha_x * alpha_y * cos4theta);
        break;
      }
      case kGGX : {
        Float tan2theta = Frame::Sin2Theta(wh) / Frame::Cos2Theta(wh);
        if (std::isinf(tan2theta)) {
          result = 0.0f;
          break;
        }
        Float cos4theta = Frame::Cos2Theta(wh) * Frame::Cos2Theta(wh);
        Float e = (Frame::Cos2Phi(wh) / (alpha_x * alpha_x) + Frame::Sin2Phi(wh) / (alpha_y * alpha_y)) * tan2theta;
        return 1 / (kPi * alpha_x * alpha_y * cos4theta * (1 + e) * (1 + e));
      }
    }
    return result;
  }

  // Sample microfacet distribution wh for full area distribution
  Normal3f Sample(const Point2 &u, const Vector3 &wo) const {
    Normal3f result;
    switch (type) {
      case kBeckmann: {
        Float tan2theta, phi;
        if (alpha_x == alpha_y) {
          Float log_sample = std::log(1 - u[0]);
          if (std::isinf(log_sample)) log_sample = 0;
          tan2theta = -alpha_x * alpha_y * std::log(1 - u[0]);
          phi = u[1] * 2 * kPi;
        } else {
          Float log_sample = std::log(u[0]);
          phi = std::atan(alpha_y / alpha_x *
              std::tan(2 * kPi * u[1] + 0.5f * kPi));
          if (u[1] > 0.5f)
            phi += kPi;
          Float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
          Float alphax2 = alpha_x * alpha_x, alphay2 = alpha_y * alpha_y;
          tan2theta = -log_sample /
              (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
        }
        Float costheta = 1 / std::sqrt(1 + tan2theta);
        Float sintheta = std::sqrt(std::max((Float)0, 1 - costheta * costheta));
        result = Normal3f(sintheta * std::cos(phi), sintheta * std::sin(phi), costheta);
        if (result.z * wo.z > 0) result *= -1;
        break;
      }
      case kGGX: {
        Float costheta = 0, phi = (2 * kPi) * u[1];
        if (alpha_x == alpha_y) {
          Float tantheta2 = alpha_x * alpha_x * u[0] / (1.0f - u[0]);
          costheta = 1 / std::sqrt(1 + tantheta2);
        } else {
          phi = std::atan(alpha_y / alpha_x * std::tan(2 * kPi * u[1] + .5f * kPi));
          if (u[1] > .5f) phi += kPi;
          Float sinphi = std::sin(phi), cosPhi = std::cos(phi);
          const Float alphax2 = alpha_x * alpha_x, alphay2 = alpha_y * alpha_y;
          const Float alpha2 = 1 / (cosPhi * cosPhi / alphax2 + sinphi * sinphi / alphay2);
          Float tantheta2 = alpha2 * u[0] / (1 - u[0]);
          costheta = 1 / std::sqrt(1 + tantheta2);
        }
        Float sintheta =
            std::sqrt(std::max((Float)0., (Float)1. - costheta * costheta));
        result = Normal3f(sintheta * std::cos(phi), sintheta * std::sin(phi), costheta);
        if (result.z * wo.z > 0) result *= -1;
      }
    }
    return result;
  }

  // Current only sample all area
  Float Pdf(const Vector3 &wo, const Vector3 &wh) const {
    return Evaluate(wh) * std::abs(Frame::CosTheta(wh));
  }

  Float SmithG1(const Vector3 &wo, const Vector3 &wh) const {
    if (Dot(wo, wh) * Frame::CosTheta(wo) <= 0.f) return 0.0f;
    Float tantheta = std::abs(Frame::TanTheta(wo));
    if (tantheta == 0.0f) return 1;
    Float alpha = ProjectRoughness(wo);
    switch (type) {
      case kGGX: {
        Float root = alpha * tantheta;
        return 2.0f / (1.0f + std::sqrt(1 + root * root));
      }
    }
  }

  Float SmithG(const Vector3 &wo, const Vector3 &wi, const Vector3 &wh) const {
    return SmithG1(wi, wh) * SmithG1(wo, wh);
  }

  inline bool IsIsotropic() const { return alpha_x == alpha_y; }

 protected:
  inline Float ProjectRoughness(const Vector3 &v) const {
    Float invSinTheta2 = 1 / Frame::Sin2Theta(v);

    if (IsIsotropic() || invSinTheta2 <= 0)
      return alpha_x;

    Float cosPhi2 = v.x * v.x * invSinTheta2;
    Float sinPhi2 = v.y * v.y * invSinTheta2;

    return std::sqrt(cosPhi2 * alpha_x * alpha_x + sinPhi2 * alpha_y * alpha_y);
  }
};

}