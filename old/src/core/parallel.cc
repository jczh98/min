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
#include <min-ray/parallel.h>

namespace min::ray {
static std::vector<std::thread> threads;
static bool shutdown_threads = false;
static ParallelForLoop *worklist = nullptr;
static std::mutex worklist_mutex;
static std::atomic<bool> report_work_states(false);
static std::atomic<int> reporter_count;
static std::condition_variable report_done_condition;
static std::mutex report_done_mutex;
static int nThread = 0;
thread_local int thread_index = 0;
static std::condition_variable worklist_condition;

void ParallelFor(WorkFunc1D func, int64_t count, int chunk_size) {
  if (threads.size() == 0 && MaxThreadIndex() != 1) return;
  if (threads.empty() || count < chunk_size) {
    for (int64_t i = 0; i < count; i++) {
      func(i);
    }
    return;
  }
  ParallelForLoop loop(std::move(func), count, chunk_size, 0);
  {
    std::lock_guard<std::mutex> lock(worklist_mutex);
    loop.next = worklist;
    worklist = &loop;
  }
  std::unique_lock<std::mutex> lock(worklist_mutex);
  worklist_condition.notify_all();
  while (!loop.finished()) {
    int64_t index_start = loop.next_index;
    int64_t index_end = std::min(index_start + loop.chunk_size, loop.max_iteration);
    loop.next_index = index_end;
    if (loop.next_index == loop.max_iteration) {
      worklist = loop.next;
    }
    ++loop.activate_workers;
    lock.unlock();
    for (int64_t index = index_start; index < index_end; ++index) {
      if (loop.func1D) {
        loop.func1D(index);
      } else {
        loop.func2D(Point2i(index % loop.num_x, index / loop.num_x));
      }
    }
    lock.lock();
    --loop.activate_workers;
  }
}

void ParallelFor2D(const WorkFunc2D &func, const Point2i &count) {
  if (threads.size() == 0 && MaxThreadIndex() != 1) return;

  if (threads.empty() || count.x * count.y <= 1) {
    for (int64_t y = 0; y < count.y; ++y) {
      for (int64_t x = 0; x < count.x; ++x) {
        func(Point2i(x, y));
      }
    }
    return;
  }

  ParallelForLoop loop(std::move(func), count, 0);
  {
    std::lock_guard<std::mutex> lock(worklist_mutex);
    loop.next = worklist;
    worklist = &loop;
  }

  std::unique_lock<std::mutex> lock(worklist_mutex);
  worklist_condition.notify_all();
  while (!loop.finished()) {
    int64_t index_start = loop.next_index;
    int64_t index_end = std::min(index_start + loop.chunk_size, loop.max_iteration);

    loop.next_index = index_end;

    if (loop.next_index == loop.max_iteration) {
      worklist = loop.next;
    }

    ++loop.activate_workers;
    lock.unlock();
    for (int64_t index = index_start; index < index_end; ++index) {
      if (loop.func1D) {
        loop.func1D(index);
      } else {
        loop.func2D(Point2i(index % loop.num_x, index / loop.num_x));
      }
    }
    lock.lock();
    --loop.activate_workers;
  }
}

static void WorkerThreadFunc(int threadindex, std::shared_ptr<Barrier> barrier) {
  thread_index = threadindex;

  barrier->Wait();

  barrier.reset();

  std::unique_lock<std::mutex> lock(worklist_mutex);
  while (!shutdown_threads) {
    if (report_work_states) {
      if (--reporter_count == 0) {
        report_done_condition.notify_one();
      }
      worklist_condition.wait(lock);
    } else if (!worklist) {
      worklist_condition.wait(lock);
    } else {
      ParallelForLoop &loop = *worklist;

      int64_t indexStart = loop.next_index;
      int64_t indexEnd = std::min(indexStart + loop.chunk_size, loop.max_iteration);

      loop.next_index = indexEnd;
      if (loop.next_index == loop.max_iteration) {
        worklist = loop.next;
      }

      ++loop.activate_workers;
      lock.unlock();
      for (int64_t index = indexStart; index < indexEnd; ++index) {
        if (loop.func1D) {
          loop.func1D(index);
        } else {
          loop.func2D(Point2i(index % loop.num_x, index / loop.num_x));
        }
      }
      lock.lock();
      --loop.activate_workers;
      if (loop.finished()) {
        worklist_condition.notify_all();
      }
    }
  }
}

void SetThreadNumber(int num) {
  nThread = num;
}

int MaxThreadIndex() {
  return nThread == 0 ? NumSystemCores() : nThread;
}

void ParallelInit(int num) {
  if (threads.size() != 0) return;
  SetThreadNumber(num);
  int nThreads = MaxThreadIndex();
  thread_index = 0;

  std::shared_ptr<Barrier> barrier = std::make_shared<Barrier>(nThreads);

  for (int i = 0; i < nThreads - 1; ++i) {
    threads.push_back(std::thread(WorkerThreadFunc, i + 1, barrier));
  }

  barrier->Wait();
}

void ParallelCleanUp() {
  if (threads.empty()) {
    return;
  }
  {
    std::lock_guard<std::mutex> lock(worklist_mutex);
    shutdown_threads = true;
    worklist_condition.notify_all();
  }
  for (std::thread &thread : threads) {
    thread.join();
  }
  threads.erase(threads.begin(), threads.end());
  shutdown_threads = false;
}

}  // namespace min::ray