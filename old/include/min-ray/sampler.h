#pragma once

#include <min-ray/interface.h>
#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <min-ray/transform.h>
#include <memory>

namespace min::ray {

class ImageBlock;

class Sampler : public Unit {
 public:
  // Create an exact clone of the current instance
  virtual std::unique_ptr<Sampler> Clone() const = 0;

  virtual void Prepare(const ImageBlock &block) = 0;

  virtual void Generate() = 0;

  virtual void Advance() = 0;

  virtual float Next1D() = 0;

  virtual Point2f Next2D() = 0;
};
MIN_INTERFACE(Sampler)

}  // namespace min::ray
