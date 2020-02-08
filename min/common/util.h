#pragma once

#include <cstring>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <type_traits>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <csignal>
#include <vector>

// Reference:
// https://github.com/taichi-dev/taichi/blob/master/taichi/common/util.h

//******************************************************************************
//                                 System State
//******************************************************************************

// Platforms

// Windows
#if defined(_WIN64)
#define MIN_PLATFORM_WINDOWS
#endif

#if defined(_WIN32) && !defined(_WIN64)
static_assert(false, "32-bit Windows systems are not supported")
#endif

// Linux
#if defined(__linux__)
#define MIN_PLATFORM_LINUX
#endif

// OSX
#if defined(__APPLE__)
#define MIN_PLATFORM_OSX
// According to https://developer.apple.com/documentation/metal?language=objc,
// Metal is supported since macOS 10.11+, so we can just assume that it is
// available for all Mac users?
#define MIN_SUPPORTS_METAL
#endif

#if (defined(MIN_PLATFORM_LINUX) || defined(MIN_PLATFORM_OSX))
#define MIN_PLATFORM_UNIX
#endif

// Compilers

// MSVC
#if defined(_MSC_VER)
#define MIN_COMPILER_MSVC
#endif

// MINGW
#if defined(__MINGW64__)
#define MIN_COMPILER_MINGW
#endif

// gcc
#if defined(__GNUC__)
#define MIN_COMPILER__GCC
#endif

// clang
#if defined(__clang__)
#define MIN_COMPILER_CLANG
#endif

#if defined(MIN_COMPILER_MSVC)
#define MIN_ALIGNED(x) __declspec(align(x))
#else
#define MIN_ALIGNED(x) __attribute__((aligned(x)))
#endif

#if __cplusplus >= 201703L
#define MIN_CPP17
#else
#if defined(MIN_COMPILER_CLANG)
static_assert(false, "For clang compilers, use -std=c++17");
#endif
static_assert(__cplusplus >= 201402L, "C++14 required.");
#define MIN_CPP14
#endif

#ifdef MIN_COMPILER_MSVC
#define NOMINMAX
#endif

// Do not disable assert...
#ifdef NDEBUG
#undef NDEBUG
#endif

#ifdef _WIN64
#define __FILENAME__ \
  (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#pragma warning(push)
#pragma warning(disable : 4005)
#include <windows.h>
#pragma warning(pop)
#include <intrin.h>
#define MIN_EXPORT __declspec(dllexport)
#else
#define __FILENAME__ \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define MIN_EXPORT
#endif

#ifndef _WIN64
#define sscanf_s sscanf
#define sprintf_s sprintf
#endif

#undef assert
#ifdef _WIN64
#ifndef MIN_PASS_EXCEPTION_TO_PYTHON
// For Visual Studio debugging...
#define DEBUG_TRIGGER __debugbreak()
#else
#define DEBUG_TRIGGER
#endif
#else
#define DEBUG_TRIGGER
#endif

#define assert_info(x, info)               \
  {                                        \
    bool ___ret___ = static_cast<bool>(x); \
    if (!___ret___) {                      \
      MIN_ERROR(info);                      \
    }                                      \
  }

#define MIN_STATIC_ASSERT(x) static_assert((x), #x);
#define MIN_ASSERT(x) MIN_ASSERT_INFO((x), #x)
#define MIN_ASSERT_INFO assert_info
#define MIN_NOT_IMPLEMENTED MIN_ERROR("Not supported.");

#define MIN_NAMESPACE_BEGIN namespace min {
#define MIN_NAMESPACE_END }

MIN_NAMESPACE_BEGIN
//******************************************************************************
//                                 Types
//******************************************************************************

using uchar = unsigned char;

using int8 = int8_t;
using uint8 = uint8_t;

using int16 = int16_t;
using uint16 = uint16_t;

using int32 = int32_t;
using uint32 = uint32_t;
using uint = unsigned int;

using int64 = int64_t;
using uint64 = uint64_t;

#ifdef _WIN64
#define MIN_FORCE_INLINE __forceinline
#else
#define MIN_FORCE_INLINE inline __attribute__((always_inline))
#endif

using float32 = float;
using float64 = double;

#ifdef MIN_USE_DOUBLE
using Float = float64;
#else
using Float = float32;
#endif

// Float literal for both float32/64
// (Learned from https://github.com/hi2p-perim/lightmetrica-v2)
Float constexpr operator"" _f(long double v) {
  return Float(v);
}
Float constexpr operator"" _f(unsigned long long v) {
  return Float(v);
}

float32 constexpr operator"" _f32(long double v) {
  return float32(v);
}
float32 constexpr operator"" _f32(unsigned long long v) {
  return float32(v);
}

float32 constexpr operator"" _fs(long double v) {
  return float32(v);
}
float32 constexpr operator"" _fs(unsigned long long v) {
  return float32(v);
}

float64 constexpr operator"" _f64(long double v) {
  return float64(v);
}
float64 constexpr operator"" _f64(unsigned long long v) {
  return float64(v);
}

float64 constexpr operator"" _fd(long double v) {
  return float64(v);
}
float64 constexpr operator"" _fd(unsigned long long v) {
  return float64(v);
}

MIN_NAMESPACE_END

//******************************************************************************
//                               Logging
//******************************************************************************
#include "spdlog/fmt/fmt.h"
namespace spdlog {
class logger;
}

MIN_NAMESPACE_BEGIN

#define SPD_AUGMENTED_LOG(X, ...)                                        \
  min::logger.X(                                                      \
      fmt::format("[{}:{}@{}] ", __FILENAME__, __FUNCTION__, __LINE__) + \
      fmt::format(__VA_ARGS__))

#if defined(MIN_PLATFORM_WINDOWS)
#define TI_UNREACHABLE __assume(0);
#else
#define TI_UNREACHABLE __builtin_unreachable();
#endif

#define MIN_TRACE(...) SPD_AUGMENTED_LOG(trace, __VA_ARGS__)
#define MIN_DEBUG(...) SPD_AUGMENTED_LOG(debug, __VA_ARGS__)
#define MIN_INFO(...) SPD_AUGMENTED_LOG(info, __VA_ARGS__)
#define MIN_WARN(...) SPD_AUGMENTED_LOG(warn, __VA_ARGS__)
#define MIN_ERROR(...)                      \
  {                                        \
    SPD_AUGMENTED_LOG(error, __VA_ARGS__); \
    TI_UNREACHABLE;                        \
  }
#define MIN_CRITICAL(...)                      \
  {                                           \
    SPD_AUGMENTED_LOG(critical, __VA_ARGS__); \
    TI_UNREACHABLE;                           \
  }

#define MIN_TRACE_IF(condition, ...) \
  if (condition) {                  \
    MIN_TRACE(__VA_ARGS__);          \
  }
#define MIN_TRACE_UNLESS(condition, ...) \
  if (!(condition)) {                   \
    MIN_TRACE(__VA_ARGS__);              \
  }
#define MIN_DEBUG_IF(condition, ...) \
  if (condition) {                  \
    MIN_DEBUG(__VA_ARGS__);          \
  }
#define MIN_DEBUG_UNLESS(condition, ...) \
  if (!(condition)) {                   \
    MIN_DEBUG(__VA_ARGS__);              \
  }
#define MIN_INFO_IF(condition, ...) \
  if (condition) {                 \
    MIN_INFO(__VA_ARGS__);          \
  }
#define MIN_INFO_UNLESS(condition, ...) \
  if (!(condition)) {                  \
    MIN_INFO(__VA_ARGS__);              \
  }
#define MIN_WARN_IF(condition, ...) \
  if (condition) {                 \
    MIN_WARN(__VA_ARGS__);          \
  }
#define MIN_WARN_UNLESS(condition, ...) \
  if (!(condition)) {                  \
    MIN_WARN(__VA_ARGS__);              \
  }
#define MIN_ERROR_IF(condition, ...) \
  if (condition) {                  \
    MIN_ERROR(__VA_ARGS__);          \
  }
#define MIN_ERROR_UNLESS(condition, ...) \
  if (!(condition)) {                   \
    MIN_ERROR(__VA_ARGS__);              \
  }
#define MIN_CRITICAL_IF(condition, ...) \
  if (condition) {                     \
    MIN_CRITICAL(__VA_ARGS__);          \
  }
#define MIN_CRITICAL_UNLESS(condition, ...) \
  if (!(condition)) {                      \
    MIN_CRITICAL(__VA_ARGS__);              \
  }

#define MIN_STOP MIN_ERROR("Stopping here")
#define MIN_TAG MIN_TRACE("Tagging here")

#define MIN_LOG_SET_PATTERN(x) spdlog::set_pattern(x);

#define MIN_FLUSH_LOGGER \
  { min::logger.flush(); };

class Logger {
  std::shared_ptr<spdlog::logger> console;

 public:
  Logger();
  void trace(const std::string &s);
  void debug(const std::string &s);
  void info(const std::string &s);
  void warn(const std::string &s);
  void error(const std::string &s, bool raise_signal = true);
  void critical(const std::string &s, bool raise_signal = true);
  void flush();
  void set_level(const std::string &level);
};

extern Logger logger;

//******************************************************************************
//                               String Utils
//******************************************************************************

inline std::vector<std::string> SplitString(const std::string &s,
                                             const std::string &seperators) {
  std::vector<std::string> ret;
  bool is_seperator[256] = {false};
  for (auto &ch : seperators) {
    is_seperator[(unsigned int)ch] = true;
  }
  int begin = 0;
  for (int i = 0; i <= (int)s.size(); i++) {
    if (is_seperator[(uint8)s[i]] || i == (int)s.size()) {
      ret.push_back(std::string(s.begin() + begin, s.begin() + i));
      begin = i + 1;
    }
  }
  return ret;
}

MIN_NAMESPACE_END