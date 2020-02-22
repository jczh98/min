#pragma once

#include <min/visual/defs.h>

namespace min {

Float FrDielectric(Float cosThetaI, Float etaI, Float etaT) {
  cosThetaI = Clamp<Float>(cosThetaI, -1, 1);
  // Potentially swap indices of refraction
  bool entering = cosThetaI > 0.f;
  if (!entering) {
    std::swap(etaI, etaT);
    cosThetaI = std::abs(cosThetaI);
  }

  // Compute _cosThetaT_ using Snell's law
  Float sinThetaI = std::sqrt(std::max((Float)0, 1 - cosThetaI * cosThetaI));
  Float sinThetaT = etaI / etaT * sinThetaI;

  // Handle total internal reflection
  if (sinThetaT >= 1) return 1;
  Float cosThetaT = std::sqrt(std::max((Float)0, 1 - sinThetaT * sinThetaT));
  Float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
      ((etaT * cosThetaI) + (etaI * cosThetaT));
  Float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
      ((etaI * cosThetaI) + (etaT * cosThetaT));
  return (Rparl * Rparl + Rperp * Rperp) / 2;
}

}


