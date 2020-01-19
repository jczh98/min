
/* =======================================================================
     This file contains classes for parallel rendering of "image blocks".
 * ======================================================================= */

#pragma once

#include <min-ray/color.h>
#include <min-ray/vector.h>
#include <tbb/mutex.h>

#define NORI_BLOCK_SIZE 32 /* Block size used for parallelization */

namespace min::ray {

/**
 * \brief Weighted pixel storage for a rectangular subregion of an image
 *
 * This class implements storage for a rectangular subregion of a
 * larger image that is being rendered. For each pixel, it records color
 * values along with a weight that specifies the accumulated influence of
 * nearby samples on the pixel (according to the used reconstruction filter).
 *
 * When rendering with filters, the samples in a rectangular
 * region will generally also contribute to pixels just outside of 
 * this region. For that reason, this class also stores information about
 * a small border region around the rectangle, whose size depends on the
 * properties of the reconstruction filter.
 */
class ImageBlock : public Eigen::Array<Color4f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> {
 public:
  /**
     * Create a new image block of the specified maximum size
     * \param size
     *     Desired maximum size of the block
     * \param filter
     *     Samples will be convolved with the image reconstruction
     *     filter provided here.
     */
  ImageBlock(const Vector2i &size, const ReconstructionFilter *filter);

  /// Release all memory
  ~ImageBlock();

  /// Configure the offset of the block within the main image
  void SetOffset(const Point2i &offset) { this->offset = offset; }

  /// Return the offset of the block within the main image
  inline const Point2i &GetOffset() const { return offset; }

  /// Configure the size of the block within the main image
  void SetSize(const Point2i &size) { this->size = size; }

  /// Return the size of the block within the main image
  inline const Vector2i &GetSize() const { return size; }

  /// Return the border size in pixels
  inline int GetBorderSize() const { return border_size; }

  /**
     * \brief Turn the block into a proper bitmap
     * 
     * This entails normalizing all pixels and discarding
     * the border region.
     */
  Bitmap *ToBitmap() const;

  /// Convert a bitmap into an image block
  void FromBitmap(const Bitmap &bitmap);

  /// Clear all contents
  void Clear() { setConstant(Color4f()); }

  /// Record a sample with the given position and radiance value
  void Put(const Point2f &pos, const Color3f &value);

  /**
     * \brief Merge another image block into this one
     *
     * During the merge operation, this function locks 
     * the destination block using a mutex.
     */
  void Put(ImageBlock &b);

  /// Lock the image block (using an internal mutex)
  inline void Lock() const { mutex.lock(); }

  /// Unlock the image block
  inline void Unlock() const { mutex.unlock(); }

  /// Return a human-readable string summary
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

/**
 * \brief Spiraling block generator
 *
 * This class can be used to chop up an image into many small
 * rectangular blocks suitable for parallel rendering. The blocks
 * are ordered in spiraling pattern so that the center is
 * rendered first.
 */
class BlockGenerator {
 public:
  /**
     * \brief Create a block generator with
     * \param size
     *      Size of the image that should be split into blocks
     * \param blockSize
     *      Maximum size of the individual blocks
     */
  BlockGenerator(const Vector2i &size, int block_size);

  /**
     * \brief Return the next block to be rendered
     *
     * This function is thread-safe
     *
     * \return \c false if there were no more blocks
     */
  bool Next(ImageBlock &block);

  /// Return the total number of blocks
  int GetBlockCount() const { return blocks_left; }

  /// Set total number of blocks (for progressive rendering)
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
