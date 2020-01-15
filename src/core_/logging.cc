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

#include <min-ray/logging.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace min::logging {

void Logger::set_level(const std::string &level) {
  if (level == "trace") {
    spdlog::set_level(spdlog::level::trace);
  } else if (level == "debug") {
    spdlog::set_level(spdlog::level::debug);
  } else if (level == "info") {
    spdlog::set_level(spdlog::level::info);
  } else if (level == "warn") {
    spdlog::set_level(spdlog::level::warn);
  } else if (level == "error") {
    spdlog::set_level(spdlog::level::err);
  } else if (level == "critical") {
    spdlog::set_level(spdlog::level::critical);
  } else if (level == "off") {
    spdlog::set_level(spdlog::level::off);
  } else {
    MIN_ERROR("Unknown logging level [{}]. Levels = trace, debug, info, warn, error, critical, off",
              level);
  }
}

Logger::Logger() {
  console = spdlog::stdout_color_mt("console");
  spdlog::set_pattern("[%L %D %X.%e] %v");
  set_level("trace");
  trace("Program started.");
}

void Logger::trace(const std::string &msg) {
  console->trace(msg);
}

void Logger::debug(const std::string &msg) {
  console->debug(msg);
}

void Logger::info(const std::string &msg) {
  console->info(msg);
}

void Logger::warn(const std::string &msg) {
  console->warn(msg);
}

void Logger::error(const std::string &msg) {
  console->error(msg);
}

void Logger::critical(const std::string &msg) {
  console->critical(msg);
}

void Logger::flush() {
  console->flush();
}

Logger logger;

}  // namespace min::logging