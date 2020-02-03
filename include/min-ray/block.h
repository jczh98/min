#pragma once

#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <tbb/mutex.h>

#define MIN_BLOCK_SIZE 32

namespace min::ray {

class ImageBlock : public Eigen::Array<Color4f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> {
 public:

  ImageBlock(const Vector2i &size, const ReconstructionFilter *filter);

  ~ImageBlock();

  void SetOffset(const Point2i &offset) { this->offset = offset; }

  inline const Point2i &GetOffset() const { return offset; }

  void SetSize(const Point2i &size) { this->size = size; }

  inline const Vector2i &GetSize() const { return size; }

  inline int GetBorderSize() const { return border_size; }

  Bitmap *ToBitmap() const;

  void FromBitmap(const Bitmap &bitmap);

  void Clear() { setConstant(Color4f()); }

  void Put(const Point2f &pos, const Color3f &value);

  void Put(ImageBlock &b);

  inline void Lock() const { mutex.lock(); }

  inline void Unlock() const { mutex.unlock(); }

  std::string ToString() const;

 protected:
  Point2i offset;
  Vector2i size;
  int border_size = 0;
  float *filter = nullptr;
  float filter_radius = 0;
  float *weights_x = nullptr;
  float *weights_y = nullptr;
  float lookup_factor = 0;
  mutable tbb::mutex mutex;
};

class BlockGenerator {
 public:

  BlockGenerator(const Vector2i &size, int block_size);

  bool Next(ImageBlock &block);

  int GetBlockCount() const { return blocks_left; }

  void SetBlockCount(const Vector2i &size, int block_size);

 protected:
  enum EDirection { ERight = 0,
                    EDown,
                    ELeft,
                    EUp };

  Point2i block;
  Vector2i num_blocks;
  Vector2i size;
  int block_size;
  int num_steps;
  int blocks_left;
  int steps_left;
  int direction;
  tbb::mutex mutex;
};

}  // namespace min::ray
