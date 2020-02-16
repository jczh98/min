#include <min/common/parallel.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

namespace min {

void ParallelFor(std::function<void(int64_t)> func, int64_t count) {
  tbb::blocked_range<int> range(0, count);
  auto map = [&](const tbb::blocked_range<int> &range) {
    for (int i = range.begin(); i < range.end(); ++i) func(i);
  };
  tbb::parallel_for(range, map);
}

void ParallelFor2D(std::function<void(Vector2i)> func, const Vector2i &count) {
  tbb::blocked_range<int> range(0, count.x * count.y);
  auto map = [&](const tbb::blocked_range<int> &range) {
    for (int i = range.begin(); i < range.end(); ++i) func(Vector2i(i % count.x, i / count.x));
  };
  tbb::parallel_for(range, map);
}

}

