#include <min/common/memory.h>
#include <corecrt_malloc.h>

namespace min {

void *AllocAligned_(size_t size) {
  return _aligned_malloc(size, 64);
}

void FreeAligned(void *ptr) {
  if (!ptr) return;
  _aligned_free(ptr);
}

}

