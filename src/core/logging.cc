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

#include <min-ray/util.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace min {

void signal_handler(int signo);

#define MIN_REGISTER_SIGNAL_HANDLER(name, handler)                   \
  {                                                                 \
    if (std::signal(name, handler) == SIG_ERR)                      \
      std::printf("Cannot register signal handler for" #name "\n"); \
  }

void Logger::set_level(const std::string &level) {
  /*
  trace = 0,
  debug = 1,
  info = 2,
  warn = 3,
  err = 4,
  critical = 5,
  off = 6
  */
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
    MIN_ERROR(
        "Unknown logging level [{}]. Levels = trace, debug, info, warn, error, "
        "critical, off",
        level);
  }
}

Logger::Logger() {
  console = spdlog::stdout_color_mt("console");
  console->flush_on(spdlog::level::trace);
  MIN_LOG_SET_PATTERN("[%L %D %X.%e] %v")

  MIN_REGISTER_SIGNAL_HANDLER(SIGSEGV, signal_handler);
  MIN_REGISTER_SIGNAL_HANDLER(SIGABRT, signal_handler);
#if !defined(_WIN64)
  MIN_REGISTER_SIGNAL_HANDLER(SIGBUS, signal_handler);
#endif
  MIN_REGISTER_SIGNAL_HANDLER(SIGFPE, signal_handler);
  spdlog::set_level(spdlog::level::trace);
  MIN_TRACE("Program started.");
}

void Logger::trace(const std::string &s) {
  console->trace(s);
}

void Logger::debug(const std::string &s) {
  console->debug(s);
}

void Logger::info(const std::string &s) {
  console->info(s);
}

void Logger::warn(const std::string &s) {
  console->warn(s);
}
void Logger::error(const std::string &s, bool raise_signal) {
  console->error(s);
  if (raise_signal) {
    std::raise(SIGABRT);
  }
}
void Logger::critical(const std::string &s, bool raise_signal) {
  console->critical(s);
  if (raise_signal) {
    std::raise(SIGABRT);
  }
}
void Logger::flush() {
  console->flush();
}

Logger logger;

std::string signal_name(int sig) {
#if !defined(_WIN64)
  return strsignal(sig);
#else
  if (sig == SIGABRT) {
    return "SIGABRT";
  } else if (sig == SIGFPE) {
    return "SIGFPE";
  } else if (sig == SIGILL) {
    return "SIGFPE";
  } else if (sig == SIGSEGV) {
    return "SIGSEGV";
  } else if (sig == SIGTERM) {
    return "SIGTERM";
  } else {
    return "SIGNAL-Unknown";
  }
#endif
}

bool python_at_exit_called = false;

void signal_handler(int signo) {
  logger.error(
      fmt::format("Received signal {} ({})", signo, signal_name(signo)), false);
  MIN_FLUSH_LOGGER;
  std::exit(-1);
}

}  // namespace min::logging