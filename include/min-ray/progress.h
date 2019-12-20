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

#include <fmt/chrono.h>
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include "defs.h"

namespace min::ray {
class ProgressBar {
 public:
  ProgressBar(uint total, uint width, char complete, char incomplete)
      : total_ticks_{total}, bar_width_{width}, complete_char_{complete}, incomplete_char_{incomplete} {}
  ProgressBar(uint total, uint width) : total_ticks_{total}, bar_width_{width} {}
  uint operator++() {
    std::lock_guard<std::mutex> lock(mutex_);
    return ticks_++;
  }

  void Display() const {
    float progress = (float)ticks_ / total_ticks_;
    int pos = (int)(bar_width_ * progress);

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_).count();
    auto time_elapsed_seconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time_) / ticks_ * total_ticks_;

    std::unique_ptr<char[]> buf(new char[total_ticks_]);
    std::unique_ptr<char[]> end_buf(new char[total_ticks_]);
    snprintf(buf.get(), total_ticks_, "\r%s: [", "Rendering");
    char *cur_space = buf.get() + strlen(buf.get());
    char *s = cur_space;
    for (int i = 0; i < bar_width_; ++i) {
      if (i < pos)
        *s++ = complete_char_;
      else if (i == pos)
        *s++ = '>';
      else
        *s++ = incomplete_char_;
    }
    *s++ = ']';
    *s++ = ' ';
    *s++ = '\0';
    auto message = fmt::format("{}%%  Elapsed {:.3f}s [{:.3f}s/it | Total {:%H:%M:%OS}]  \r",
                               int(progress * 100.0), float(time_elapsed) / 1000.0,
                               float(time_elapsed) / 1000.0 / ticks_,
                               time_elapsed_seconds);
    snprintf(end_buf.get(), total_ticks_, message.c_str());
    fputs(buf.get(), stdout);
    fputs(end_buf.get(), stdout);
    fflush(stdout);
  }

  void Done() const {
    Display();
    std::cout << std::endl;
  }

 private:
  std::mutex mutex_;
  std::atomic<unsigned int> ticks_ = 0;
  const uint total_ticks_;
  const uint bar_width_;
  const char complete_char_ = '=';
  const char incomplete_char_ = ' ';
  const std::chrono::steady_clock::time_point start_time_ = std::chrono::steady_clock::now();
};
}  // namespace min::ray