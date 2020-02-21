#pragma once

#include "defs.h"
#include "spectrum.h"
#include "frame.h"

namespace min {

class BxDF {
 public:
  enum Type {
    kNone = 0,
    kDiffuse = 1ULL,
    kGlossy = 1ULL << 1U,
    kReflection = 1ULL << 2U,
    kTransmission = 1ULL << 3U,
    kSpecular = 1ULL << 4U,
    kAll = kDiffuse | kGlossy | kTransmission | kSpecular | kReflection,
    kAllButSpecular = kAll & ~kSpecular
  };
  const Type type;
  explicit BxDF(Type type) : type(type) {}
  bool MatchesFlags(Type t) const { return (type & t) == type; }
  virtual Spectrum Evaluate(const Vector3f &wo, const Vector3f &wi) const = 0;
  virtual void Sample(const Point2f &u, const Vector3f &wo, BSDFSample &sample) const;
  virtual Float Pdf(const Vector3 &wo, const Vector3 &wi) const;
};

struct BSDFSample {
  Vector3f wi;
  Float pdf;
  Spectrum f;
  BxDF::Type sampled_type = BxDF::Type::kNone;
};

class BSDF {
 public:
  BSDF(const Frame geo_frame, const Frame shading_frame) : geo_frame(geo_frame), shading_frame(shading_frame) {}
  void Add(const std::shared_ptr<BxDF> &b) {
    MIN_ASSERT(num_bxdfs <= kMaxBxDFs);
    num_bxdfs++;
    bxdfs.emplace_back(b);
  }
  int NumComponents(BxDF::Type flags = BxDF::Type::kAll) const {
    int num = 0;
    for (int i = 0; i < num_bxdfs; ++i) {
      if (bxdfs[i]->MatchesFlags(flags)) ++num;
    }
    return num;
  }
  Vector3f WorldToLocal(const Vector3 &v) const {
    return shading_frame.ToLocal(v);
  }
  Vector3f LocalToWorld(const Vector3 &v) const {
    return shading_frame.ToWorld(v);
  }
  Spectrum Evaluate(const Vector3 &wow, const Vector3 &wiw, BxDF::Type flags = BxDF::Type::kAll) const {
    Vector3 wi = WorldToLocal(wiw), wo = WorldToLocal(wow);
    if (wo.z == 0.f) return Spectrum(0);
    bool reflect = Dot(wiw, geo_frame.n) * Dot(wow, geo_frame.n) > 0;
    Spectrum f(0);
    for (int i = 0; i < num_bxdfs; i++) {
      if (bxdfs[i]->MatchesFlags(flags) &&
          ((reflect && (bxdfs[i]->type & BxDF::Type::kReflection)) ||
              (!reflect && (bxdfs[i]->type & BxDF::Type::kTransmission)))) {
        f += bxdfs[i]->Evaluate(wo, wi);
      }
    }
    return f;
  }
  void Sample(const Point2 &u, const Vector3 &wow, BSDFSample &sample, BxDF::Type type = BxDF::Type::kAll) const {
    int matching_comps = NumComponents(type);
    // Choose which BxDF to sample
    if (matching_comps == 0) {
      sample.pdf = 0;
      sample.sampled_type = BxDF::Type::kNone;
      sample.f = Spectrum(0);
      return;
    }
    int comp = std::min((int)std::floor(u[0] * matching_comps), matching_comps - 1);
    std::shared_ptr<BxDF> bxdf = nullptr;
    int count = comp;
    for (int i = 0; i < num_bxdfs; i++) {
      if (bxdfs[i]->MatchesFlags(type) && count-- == 0) {
        bxdf = bxdfs[i];
        break;
      }
    }
    MIN_ASSERT(bxdf != nullptr);
    Point2 u_remapped(std::min(u[0] * matching_comps - comp, kOneMinusEpsilon), u[1]);
    Vector3f wi, wo = WorldToLocal(wow);
    if (wo.z == 0.0f)  {
      sample.f = Spectrum(0);
      return;
    }
    sample.sampled_type = bxdf->type;
    bxdf->Sample(u_remapped, wo, sample);
    if (sample.pdf == 0) {
      sample.sampled_type = BxDF::Type::kNone;
      sample.f = Spectrum(0);
      return;
    }
    wi = sample.wi;
    sample.wi = LocalToWorld(wi);
    // Compute overall
    if (!(bxdf->type & BxDF::Type::kSpecular) && matching_comps > 1) {
      for (int i = 0; i < num_bxdfs; i++) {
        if (bxdfs[i] != bxdf && bxdfs[i]->MatchesFlags(type)) {
          sample.pdf += bxdfs[i]->Pdf(wo, wi);
        }
      }
    }
    if (matching_comps > 1) sample.pdf /= matching_comps;
    if (!bxdf->type & BxDF::Type::kSpecular) {
      bool reflect = Dot(sample.wi, geo_frame.n) * Dot(wow, geo_frame.n) > 0;
      sample.f = Spectrum(0);
      for (int i = 0; i < num_bxdfs; i++) {
        if (bxdfs[i]->MatchesFlags(type) &&
            ((reflect && (bxdfs[i]->type & BxDF::Type::kReflection)) ||
                (!reflect && (bxdfs[i]->type & BxDF::Type::kTransmission)))) {
          sample.f += bxdfs[i]->Evaluate(wo, wi);
        }
      }
    }
  }
  Float Pdf(const Vector3 &wow, const Vector3 &wiw, BxDF::Type flags = BxDF::Type::kAll) const {
    if (num_bxdfs == 0.0f) return 0.0f;
    Vector3 wo = WorldToLocal(wow), wi = WorldToLocal(wiw);
    if (wo.z == 0) return 0;
    Float pdf = 0;
    int matching_comps = 0;
    for (int i = 0; i < num_bxdfs; i++) {
      if (bxdfs[i]->MatchesFlags(flags)) {
        ++matching_comps;
        pdf += bxdfs[i]->Pdf(wo, wi);
      }
    }
    Float v = matching_comps > 0 ? pdf / matching_comps : 0;
    return v;
  }
 private:
  const Frame shading_frame, geo_frame;
  int num_bxdfs = 0;
  static constexpr int kMaxBxDFs = 8;
  std::vector<std::shared_ptr<BxDF>> bxdfs;
};

class Material : public Unit {
 public:
  virtual void ComputeScatteringEvents(SurfaceIntersection &si) const = 0;
};
MIN_INTERFACE(Material)

}

