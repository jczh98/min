#pragma once

#include <min/common/memory.h>
#include "defs.h"

namespace min {

enum ImageWrapMode { kRepeat, kBlack, kClamp};

template <typename T, int logBlockSize = 2>
class BlockedArray {
 public:
  // BlockedArray Public Methods
  BlockedArray(int uRes, int vRes, const T *d = nullptr)
      : uRes(uRes), vRes(vRes), uBlocks(RoundUp(uRes) >> logBlockSize) {
    int nAlloc = RoundUp(uRes) * RoundUp(vRes);
    data = AllocAligned<T>(nAlloc);
    for (int i = 0; i < nAlloc; ++i) new (&data[i]) T();
    if (d)
      for (int v = 0; v < vRes; ++v)
        for (int u = 0; u < uRes; ++u) (*this)(u, v) = d[v * uRes + u];
  }
  constexpr int BlockSize() const { return 1 << logBlockSize; }
  int RoundUp(int x) const {
    return (x + BlockSize() - 1) & ~(BlockSize() - 1);
  }
  int uSize() const { return uRes; }
  int vSize() const { return vRes; }
  ~BlockedArray() {
    for (int i = 0; i < uRes * vRes; ++i) data[i].~T();
    FreeAligned(data);
  }
  int Block(int a) const { return a >> logBlockSize; }
  int Offset(int a) const { return (a & (BlockSize() - 1)); }
  T &operator()(int u, int v) {
    int bu = Block(u), bv = Block(v);
    int ou = Offset(u), ov = Offset(v);
    int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
    offset += BlockSize() * ov + ou;
    return data[offset];
  }
  const T &operator()(int u, int v) const {
    int bu = Block(u), bv = Block(v);
    int ou = Offset(u), ov = Offset(v);
    int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
    offset += BlockSize() * ov + ou;
    return data[offset];
  }
  void GetLinearArray(T *a) const {
    for (int v = 0; v < vRes; ++v)
      for (int u = 0; u < uRes; ++u) *a++ = (*this)(u, v);
  }

 private:
  // BlockedArray Private Data
  T *data;
  const int uRes, vRes, uBlocks;
};

struct ResampleWeight {
  int first_texel;
  Float weight[4];
};

class MIPMap {
 private:
  const ImageWrapMode wrap_mode;
  Point2i resolution;
  std::vector<std::unique_ptr<BlockedArray<Vector3>>> pyramid;
 public:
  MIPMap(const Point2i res, const Vector3 *img, ImageWrapMode wrap_mode = ImageWrapMode::kRepeat)
    : resolution(res), wrap_mode(wrap_mode) {
    std::unique_ptr<Vector3[]> resampled_image = nullptr;
    if (!IsPowerOf2(resolution[0]) || !IsPowerOf2(resolution[1])) {
      Point2i res_resampled(RoundUpPow2(resolution[0]), RoundUpPow2(resolution[1]));
      MIN_INFO("Resampling MIPMap from {} to {}. Ratio={}", resolution.ToString(), res_resampled.ToString(),
          Float(res_resampled.x * res_resampled.y) / Float(resolution.x * resolution.y));
      // Resample in s direction
      std::unique_ptr<ResampleWeight[]> s_weights =
          ResampleWeights(resolution[0], res_resampled[0]);
      resampled_image.reset(new Vector3[res_resampled[0] * res_resampled[1]]);
      for (int t = 0; t < resolution[1]; t++) {
        for (int s = 0; s < res_resampled[0]; ++s) {
          // Compute texel $(s,t)$ in $s$-zoomed image
          resampled_image[t * res_resampled[0] + s] = Vector3(0.f);
          for (int j = 0; j < 4; ++j) {
            int origS = s_weights[s].first_texel + j;
            if (wrap_mode == ImageWrapMode::kRepeat)
              origS = Mod(origS, resolution[0]);
            else if (wrap_mode == ImageWrapMode::kClamp)
              origS = Clamp(origS, 0, resolution[0] - 1);
            if (origS >= 0 && origS < (int)resolution[0])
              resampled_image[t * res_resampled[0] + s] +=
                  s_weights[s].weight[j] *
                      img[t * resolution[0] + origS];
          }
        }
      }
      // Resample in t direction
      std::unique_ptr<ResampleWeight[]> t_weights =
          ResampleWeights(resolution[1], res_resampled[1]);
      for (int s = 0; s < res_resampled[0]; s++) {
        std::unique_ptr<Vector3[]> work_data(new Vector3[res_resampled[1]]);
        for (int t = 0; t < res_resampled[1]; ++t) {
          work_data[t] = Vector3(0.f);
          for (int j = 0; j < 4; ++j) {
            int offset = t_weights[t].first_texel + j;
            if (wrap_mode == ImageWrapMode::kRepeat)
              offset = Mod(offset, resolution[1]);
            else if (wrap_mode == ImageWrapMode::kClamp)
              offset = Clamp(offset, 0, (int)resolution[1] - 1);
            if (offset >= 0 && offset < (int)resolution[1])
              work_data[t] += t_weights[t].weight[j] *
                  resampled_image[offset * res_resampled[0] + s];
          }
        }
        for (int t = 0; t < res_resampled[1]; ++t)
          resampled_image[t * res_resampled[0] + s] = Clamp(work_data[t], Vector3(0.f), Vector3(kInfinity));
      }
      resolution = res_resampled;
    }
    // Initialize levels of details
    int levels = 1 + Log2Int(std::max(resolution[0], resolution[1]));
    pyramid.resize(levels);
    // Initialize most detailed level of MIPMap
    pyramid[0].reset(
        new BlockedArray<Vector3>(resolution[0], resolution[1],
                            resampled_image ? resampled_image.get() : img));
    for (int i = 1; i < levels; ++i) {
      // Initialize $i$th MIPMap level from $i-1$st level
      int sRes = std::max(1, pyramid[i - 1]->uSize() / 2);
      int tRes = std::max(1, pyramid[i - 1]->vSize() / 2);
      pyramid[i].reset(new BlockedArray<Vector3>(sRes, tRes));

      // Filter four texels from finer level of pyramid
      for (int t = 0; t < tRes; t++) {
        for (int s = 0; s < sRes; ++s)
          (*pyramid[i])(s, t) =
              .25f * (Texel(i - 1, 2 * s, 2 * t) +
                  Texel(i - 1, 2 * s + 1, 2 * t) +
                  Texel(i - 1, 2 * s, 2 * t + 1) +
                  Texel(i - 1, 2 * s + 1, 2 * t + 1));
      }
    }
  }

  Vector3 Lookup(const Point2f &st, Float width = 0.f) const {
    // Compute MIPMap level for trilinear filtering
    Float level = Levels() - 1 + Log2(std::max(width, (Float)1e-8));

    // Perform trilinear interpolation at appropriate MIPMap level
    if (level < 0)
      return Triangle(0, st);
    else if (level >= Levels() - 1)
      return Texel(Levels() - 1, 0, 0);
    else {
      int iLevel = std::floor(level);
      Float delta = level - iLevel;
      return Lerp(delta, Triangle(iLevel, st), Triangle(iLevel + 1, st));
    }
  }

  const Vector3 &Texel(int level, int s, int t) const {
    MIN_ASSERT(level < pyramid.size());
    const BlockedArray<Vector3> &l = *pyramid[level];
    // Compute texel $(s,t)$ accounting for boundary conditions
    switch (wrap_mode) {
      case ImageWrapMode::kRepeat:
        s = Mod(s, l.uSize());
        t = Mod(t, l.vSize());
        break;
      case ImageWrapMode::kClamp:
        s = Clamp(s, 0, l.uSize() - 1);
        t = Clamp(t, 0, l.vSize() - 1);
        break;
      case ImageWrapMode::kBlack: {
        static const Vector3 black = Vector3(0.f);
        if (s < 0 || s >= (int)l.uSize() || t < 0 || t >= (int)l.vSize())
          return black;
        break;
      }
    }
    return l(s, t);
  }
  int Width() const { return resolution[0]; }
  int Height() const { return resolution[1]; }
  int Levels() const { return pyramid.size(); }
 private:
  Float Lanczos(Float x, Float tau = 2.0f) {
    x = std::abs(x);
    if (x < 1e-5f) return 1;
    if (x > 1.f) return 0;
    x *= kPi;
    Float s = std::sin(x * tau) / (x * tau);
    Float lanczos = std::sin(x) / x;
    return s * lanczos;
  }

  Vector3 Triangle(int level, const Point2f &st) const {
    level = Clamp(level, 0, Levels() - 1);
    Float s = st[0] * pyramid[level]->uSize() - 0.5f;
    Float t = st[1] * pyramid[level]->vSize() - 0.5f;
    int s0 = std::floor(s), t0 = std::floor(t);
    Float ds = s - s0, dt = t - t0;
    return (1 - ds) * (1 - dt) * Texel(level, s0, t0) +
        (1 - ds) * dt * Texel(level, s0, t0 + 1) +
        ds * (1 - dt) * Texel(level, s0 + 1, t0) +
        ds * dt * Texel(level, s0 + 1, t0 + 1);
  }

  std::unique_ptr<ResampleWeight[]> ResampleWeights(int oldRes, int newRes) {
    MIN_ASSERT(newRes >= oldRes);
    std::unique_ptr<ResampleWeight[]> wt(new ResampleWeight[newRes]);
    Float filterwidth = 2.f;
    for (int i = 0; i < newRes; ++i) {
      // Compute image resampling weights for _i_th texel
      Float center = (i + .5f) * oldRes / newRes;
      wt[i].first_texel = std::floor((center - filterwidth) + 0.5f);
      for (int j = 0; j < 4; ++j) {
        Float pos = wt[i].first_texel + j + .5f;
        wt[i].weight[j] = Lanczos((pos - center) / filterwidth);
      }
      // Normalize filter weights for texel resampling
      Float invSumWts = 1 / (wt[i].weight[0] + wt[i].weight[1] +
          wt[i].weight[2] + wt[i].weight[3]);
      for (int j = 0; j < 4; ++j) wt[i].weight[j] *= invSumWts;
    }
    return wt;
  }
};

}
