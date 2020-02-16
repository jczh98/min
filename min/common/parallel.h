#pragma once

#include "util.h"
#include <min/math/math.h>

namespace min {

void ParallelFor(std::function<void(int64_t)> func, int64_t count);

void ParallelFor2D(std::function<void(Vector2i)> func, const Vector2i &count);

}


