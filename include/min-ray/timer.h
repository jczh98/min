
#pragma once

#include <min-ray/common.h>
#include <chrono>

namespace min::ray {

/**
 * \brief Simple timer with millisecond precision
 *
 * This class is convenient for collecting performance data
 */
class Timer {
 public:
  /// Create a new timer and reset it
  Timer() { Reset(); }

  /// Reset the timer to the current time
  void Reset() { start = std::chrono::system_clock::now(); }

  /// Return the number of milliseconds elapsed since the timer was last reset
  double Elapsed() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    return (double)duration.count();
  }

  /// Like \ref elapsed(), but return a human-readable string
  std::string ElapsedString(bool precise = false) const {
    return timeString(Elapsed(), precise);
  }

  /// Return the number of milliseconds elapsed since the timer was last reset and then reset it
  double Lap() {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    start = now;
    return (double)duration.count();
  }

  /// Like \ref lap(), but return a human-readable string
  std::string LapString(bool precise = false) {
    return timeString(Lap(), precise);
  }

 private:
  std::chrono::system_clock::time_point start;
};

}  // namespace min::ray
