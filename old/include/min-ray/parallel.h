// MIT License
//
// Copyright (c) 2018-2019, neverfelly <neverfelly@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>
#include "defs.h"
#include "math.h"

namespace min::ray {

class Barrier {
 public:
  Barrier(int count) : count_(count) {}
  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (--count_ == 0) {
      cv_.notify_all();
    } else {
      cv_.wait(lock, [this] { return count_ == 0; });
    }
  }

 private:
  std::mutex mutex_;
  std::condition_variable cv_;
  int count_;
};

using WorkFunc1D = std::function<void(int64_t)>;
using WorkFunc2D = std::function<void(Point2i)>;

class ParallelForLoop {
 public:
  ParallelForLoop(WorkFunc1D func1d, int64_t max_iter, int chunk_size, uint64_t profiler)
      : func1D(func1d), max_iteration(max_iter), chunk_size(chunk_size), profiler_state(profiler) {
  }
  ParallelForLoop(const WorkFunc2D &func2d, const Point2i &count, uint64_t profiler)
      : func2D(func2d), max_iteration(count.x * count.y), chunk_size(1), profiler_state(profiler) {
    num_x = count.x;
  }
  bool finished() const { return next_index >= max_iteration && activate_workers == 0; }
  WorkFunc1D func1D;
  WorkFunc2D func2D;
  const int64_t max_iteration;
  const int chunk_size;  // iterations each worker
  uint64_t profiler_state;
  uint64_t next_index = 0;
  int activate_workers = 0;
  ParallelForLoop *next = nullptr;
  int num_x = -1;
};

extern thread_local int thread_index;

inline int NumSystemCores() {
  return std::max(1u, std::thread::hardware_concurrency());
}

int MaxThreadIndex();

void ParallelFor(WorkFunc1D func, int64_t count, int chuck_size = 1);

void ParallelFor2D(const WorkFunc2D &func, const Point2i &count);

void SetThreadNumber(int num);

void ParallelInit(int num);

void ParallelCleanUp();

}  // namespace min::ray