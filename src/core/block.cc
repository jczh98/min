
#include <min-ray/bbox.h>
#include <min-ray/bitmap.h>
#include <min-ray/block.h>
#include <min-ray/rfilter.h>
#include <tbb/tbb.h>

namespace min::ray {

ImageBlock::ImageBlock(const Vector2i &local_size, const ReconstructionFilter *local_filter)
    : offset(0, 0), size(local_size) {
  if (local_filter) {
    /* Tabulate the image reconstruction filter for performance reasons */
    filter_radius = local_filter->GetRadius();
    border_size = (int)std::ceil(filter_radius - 0.5f);
    this->filter = new float[NORI_FILTER_RESOLUTION + 1];
    for (int i = 0; i < NORI_FILTER_RESOLUTION; ++i) {
      float pos = (filter_radius * i) / NORI_FILTER_RESOLUTION;
      this->filter[i] = local_filter->Evaluate(pos);
    }
    this->filter[NORI_FILTER_RESOLUTION] = 0.0f;
    lookup_factor = NORI_FILTER_RESOLUTION / filter_radius;
    int weightSize = (int)std::ceil(2 * filter_radius) + 1;
    weights_x = new float[weightSize];
    weights_y = new float[weightSize];
    memset(weights_x, 0, sizeof(float) * weightSize);
    memset(weights_y, 0, sizeof(float) * weightSize);
  }

  /* Allocate space for pixels and border regions */
  resize(size.y() + 2 * border_size, size.x() + 2 * border_size);
}

ImageBlock::~ImageBlock() {
  delete[] filter;
  delete[] weights_x;
  delete[] weights_y;
}

Bitmap *ImageBlock::ToBitmap() const {
  Bitmap *result = new Bitmap(size);
  for (int y = 0; y < size.y(); ++y)
    for (int x = 0; x < size.x(); ++x)
      result->coeffRef(y, x) = coeff(y + border_size, x + border_size).DivideByFilterWeight();
  return result;
}

void ImageBlock::FromBitmap(const Bitmap &bitmap) {
  if (bitmap.cols() != cols() || bitmap.rows() != rows())
    throw NoriException("Invalid bitmap dimensions!");

  for (int y = 0; y < size.y(); ++y)
    for (int x = 0; x < size.x(); ++x)
      coeffRef(y, x) << bitmap.coeff(y, x), 1;
}

void ImageBlock::Put(const Point2f &_pos, const Color3f &value) {
  if (!value.Valid()) {
    /* If this happens, go fix your code instead of removing this warning ;) */
    cerr << "Integrator: computed an invalid radiance value: " << value.ToString() << endl;
    return;
  }

  /* Convert to pixel coordinates within the image block */
  Point2f pos(
      _pos.x() - 0.5f - (offset.x() - border_size),
      _pos.y() - 0.5f - (offset.y() - border_size));

  /* Compute the rectangle of pixels that will need to be updated */
  BoundingBox2i bbox(
      Point2i((int)std::ceil(pos.x() - filter_radius), (int)std::ceil(pos.y() - filter_radius)),
      Point2i((int)std::floor(pos.x() + filter_radius), (int)std::floor(pos.y() + filter_radius)));
  bbox.Clip(BoundingBox2i(Point2i(0, 0), Point2i((int)cols() - 1, (int)rows() - 1)));

  /* Lookup values from the pre-rasterized filter */
  for (int x = bbox.min.x(), idx = 0; x <= bbox.max.x(); ++x)
    weights_x[idx++] = filter[(int)(std::abs(x - pos.x()) * lookup_factor)];
  for (int y = bbox.min.y(), idx = 0; y <= bbox.max.y(); ++y)
    weights_y[idx++] = filter[(int)(std::abs(y - pos.y()) * lookup_factor)];

  for (int y = bbox.min.y(), yr = 0; y <= bbox.max.y(); ++y, ++yr)
    for (int x = bbox.min.x(), xr = 0; x <= bbox.max.x(); ++x, ++xr)
      coeffRef(y, x) += Color4f(value) * weights_x[xr] * weights_y[yr];
}

void ImageBlock::Put(ImageBlock &b) {
  Vector2i local_offset = b.GetOffset() - offset +
                    Vector2i::Constant(border_size - b.GetBorderSize());
  Vector2i size = b.GetSize() + Vector2i(2 * b.GetBorderSize());

  tbb::mutex::scoped_lock lock(mutex);

  block(local_offset.y(), local_offset.x(), size.y(), size.x()) += b.topLeftCorner(size.y(), size.x());
}

std::string ImageBlock::ToString() const {
  return tfm::format("ImageBlock[offset=%s, size=%s]]",
                     offset.ToString(), size.ToString());
}

BlockGenerator::BlockGenerator(const Vector2i &local_size, int blockSize)
    : size(local_size), block_size(blockSize) {
  num_blocks = Vector2i(
      (int)std::ceil(local_size.x() / (float)blockSize),
      (int)std::ceil(local_size.y() / (float)blockSize));
  blocks_left = num_blocks.x() * num_blocks.y();
  direction = ERight;
  block = Point2i(num_blocks / 2);
  steps_left = 1;
  num_steps = 1;
}

void BlockGenerator::SetBlockCount(const Vector2i &local_size, int blockSize) {
  this->size = local_size;
  block_size = blockSize;
  num_blocks = Vector2i(
      (int)std::ceil(size.x() / (float)blockSize),
      (int)std::ceil(size.y() / (float)blockSize));
  blocks_left = num_blocks.x() * num_blocks.y();
  direction = ERight;
  block = Point2i(num_blocks / 2);
  steps_left = 1;
  num_steps = 1;
}

bool BlockGenerator::Next(ImageBlock &local_block) {
  tbb::mutex::scoped_lock lock(mutex);

  if (blocks_left == 0)
    return false;

  Point2i pos = this->block * block_size;
  local_block.SetOffset(pos);
  local_block.SetSize((size - pos).cwiseMin(Vector2i::Constant(block_size)));

  if (--blocks_left == 0)
    return true;

  do {
    switch (direction) {
      case ERight:
        ++this->block.x();
        break;
      case EDown:
        ++this->block.y();
        break;
      case ELeft:
        --this->block.x();
        break;
      case EUp:
        --this->block.y();
        break;
    }

    if (--steps_left == 0) {
      direction = (direction + 1) % 4;
      if (direction == ELeft || direction == ERight)
        ++num_steps;
      steps_left = num_steps;
    }
  } while ((this->block.array() < 0).any() ||
           (this->block.array() >= num_blocks.array()).any());

  return true;
}

}  // namespace min::ray
