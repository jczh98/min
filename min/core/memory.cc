#include <min/common/memory.h>
#include <cstddef>
#include <corecrt_malloc.h>

namespace min {

void *AllocAligned(size_t size) {
  return _aligned_malloc(size, 64);
}

}

