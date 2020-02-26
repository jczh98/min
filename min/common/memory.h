#pragma once

namespace min {

void *AllocAligned(size_t size);
template <typename T>
T *AllocAligned(size_t count) {
  return (T *)AllocAligned(count * size(T));
}

}


