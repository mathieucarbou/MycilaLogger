// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou
 */
#pragma once

#include <Arduino.h>
#include <Print.h>
#include <StreamString.h>
#include <esp32-hal-log.h>
#include <vector>

#define MYCILA_LOGGER_VERSION "3.2.0"
#define MYCILA_LOGGER_VERSION_MAJOR 3
#define MYCILA_LOGGER_VERSION_MINOR 2
#define MYCILA_LOGGER_VERSION_REVISION 0

#ifndef MYCILA_LOGGER_BUFFER_SIZE
#define MYCILA_LOGGER_BUFFER_SIZE 256
#endif

namespace Mycila {
  class Logger {
    public:
      uint8_t getLevel() const { return _level; }
      void setLevel(uint8_t level) { _level = level; }

      void forwardTo(Print* printer) { _outputs.push_back(printer); }

      bool isDebugEnabled() const { return getLevel() >= ARDUHAL_LOG_LEVEL_DEBUG; }

      std::vector<Print*>& getOutputs() { return _outputs; }

      template <typename... Args>
      void debug(const char* tag, const char* format, Args... args) { log(ARDUHAL_LOG_LEVEL_DEBUG, tag, format, args...); }

      template <typename... Args>
      void info(const char* tag, const char* format, Args... args) { log(ARDUHAL_LOG_LEVEL_INFO, tag, format, args...); }

      template <typename... Args>
      void warn(const char* tag, const char* format, Args... args) { log(ARDUHAL_LOG_LEVEL_WARN, tag, format, args...); }

      template <typename... Args>
      void error(const char* tag, const char* format, Args... args) { log(ARDUHAL_LOG_LEVEL_ERROR, tag, format, args...); }

      template <typename... Args>
      void log(uint8_t level, const char* tag, const char* format, Args... args) {
#ifdef MYCILA_LOGGER_CUSTOM_LEVEL
        if (level > getLevel())
          return;
#else
        if (level > _level)
          return;
#endif

        StreamString buffer;
        buffer.reserve(MYCILA_LOGGER_BUFFER_SIZE);

#if CONFIG_ARDUHAL_LOG_COLORS
        buffer.print(_colors[level]);
#endif

        buffer.printf("[%6" PRIu32 "][%c][%s][%d][%s] ",
                      static_cast<uint32_t>(esp_timer_get_time() >> 10),
                      _codes[level],
                      tag,
                      xPortGetCoreID(),
                      pcTaskGetName(NULL));
        buffer.printf(format, args...);

#if CONFIG_ARDUHAL_LOG_COLORS
        buffer.print("\033[0m");
#endif

        buffer.print("\r\n");

        for (auto& output : _outputs)
          output->print(buffer);
      }

    private:
#ifndef MYCILA_LOGGER_CUSTOM_LEVEL
      uint32_t _level = ARDUHAL_LOG_LEVEL;
#endif
      std::vector<Print*> _outputs;
      const char* _codes = " EWID";
#if CONFIG_ARDUHAL_LOG_COLORS
      const char* _colors[5] = {"", ARDUHAL_LOG_COLOR_E, ARDUHAL_LOG_COLOR_W, ARDUHAL_LOG_COLOR_I, ARDUHAL_LOG_COLOR_D};
#endif
  };
} // namespace Mycila
