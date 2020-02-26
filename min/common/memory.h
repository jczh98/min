#pragma once

#include <cstddef>

namespace min {

void *AllocAligned_(size_t size);
template <typename T>
T *AllocAligned(size_t count) {
  return (T *)AllocAligned_(count * sizeof(T));
}

void FreeAligned(void *ptr);

}


