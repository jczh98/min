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

#include <memory>
#include "spdlog/fmt/fmt.h"

namespace spdlog {
class logger;
}

namespace min::logging {

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

#define SPD_AUGMENTED_LOG(X, ...)                                        \
  min::logging::logger.X(                                                \
      fmt::format("[{}:{}@{}] ", __FILENAME__, __FUNCTION__, __LINE__) + \
      fmt::format(__VA_ARGS__))

#define MIN_TRACE(...) SPD_AUGMENTED_LOG(trace, __VA_ARGS__)
#define MIN_DEBUG(...) SPD_AUGMENTED_LOG(debug, __VA_ARGS__)
#define MIN_INFO(...) SPD_AUGMENTED_LOG(info, __VA_ARGS__)
#define MIN_WARN(...) SPD_AUGMENTED_LOG(warn, __VA_ARGS__)
#define MIN_ERROR(...) SPD_AUGMENTED_LOG(error, __VA_ARGS__)
#define MIN_CRITICAL(...) SPD_AUGMENTED_LOG(critical, __VA_ARGS__)

#define assert_info(x, info)               \
  {                                        \
    bool ___ret___ = static_cast<bool>(x); \
    if (!___ret___) {                      \
      MIN_ERROR(info);                      \
    }                                      \
  }
#define MIN_NOT_IMPLEMENTED MIN_ERROR("Not Implemented.");

class Logger {
  std::shared_ptr<spdlog::logger> console;

 public:
  Logger();
  void trace(const std::string &msg);
  void debug(const std::string &msg);
  void info(const std::string &msg);
  void warn(const std::string &msg);
  void error(const std::string &msg);
  void critical(const std::string &msg);
  void flush();
  void set_level(const std::string &level);
};

extern Logger logger;

}  // namespace min::logging