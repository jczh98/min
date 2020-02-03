
#pragma once

#include <min-ray/common.h>
#include <chrono>

namespace min::ray {

class Timer {
 public:

  Timer() { Reset(); }

  void Reset() { start = std::chrono::system_clock::now(); }

  double Elapsed() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    return (double)duration.count();
  }

  std::string ElapsedString(bool precise = false) const {
    return TimeString(Elapsed(), precise);
  }

  // Return the number of milliseconds elapsed since the timer was last reset and then reset it
  double Lap() {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    start = now;
    return (double)duration.count();
  }

  std::string LapString(bool precise = false) {
    return TimeString(Lap(), precise);
  }

 private:
  std::chrono::system_clock::time_point start;
};

}  // namespace min::ray
