// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#pragma once

#include <StreamUtils.h>
#include <vector>

#define MYCILA_LOGGER_VERSION "1.0.0"
#define MYCILA_LOGGER_VERSION_MAJOR 1
#define MYCILA_LOGGER_VERSION_MINOR 0
#define MYCILA_LOGGER_VERSION_REVISION 0

#define MYCILA_LOGGER_TASK_NAME_LENGTH 10
#define MYCILA_LOGGER_TAG_LENGTH 10

namespace Mycila {
  class LoggerClass {
    public:
      // To be implemented by the user.
      // Returns the current logging level, which could be sourced from a config system or a macro
      uint8_t getLevel() const;

    public:
      void forwardTo(Print* printer) { _outputs.push_back(printer); }

      bool isDebugEnabled() const { return getLevel() >= ARDUHAL_LOG_LEVEL_DEBUG; }

      template <typename... Args>
      void debug(const char* tag, const __FlashStringHelper* format, Args... args) { log(ARDUHAL_LOG_LEVEL_DEBUG, tag, reinterpret_cast<const char*>(format), args...); }

      template <typename... Args>
      void info(const char* tag, const __FlashStringHelper* format, Args... args) { log(ARDUHAL_LOG_LEVEL_INFO, tag, reinterpret_cast<const char*>(format), args...); }

      template <typename... Args>
      void warn(const char* tag, const __FlashStringHelper* format, Args... args) { log(ARDUHAL_LOG_LEVEL_WARN, tag, reinterpret_cast<const char*>(format), args...); }

      template <typename... Args>
      void error(const char* tag, const __FlashStringHelper* format, Args... args) { log(ARDUHAL_LOG_LEVEL_ERROR, tag, reinterpret_cast<const char*>(format), args...); }

      template <typename... Args>
      void debug(const char* tag, const char* format, Args... args) { log(ARDUHAL_LOG_LEVEL_DEBUG, tag, format, args...); }

      template <typename... Args>
      void info(const char* tag, const char* format, Args... args) { log(ARDUHAL_LOG_LEVEL_INFO, tag, format, args...); }

      template <typename... Args>
      void warn(const char* tag, const char* format, Args... args) { log(ARDUHAL_LOG_LEVEL_WARN, tag, format, args...); }

      template <typename... Args>
      void error(const char* tag, const char* format, Args... args) { log(ARDUHAL_LOG_LEVEL_ERROR, tag, format, args...); }

    private:
      template <typename... Args>
      void log(uint8_t level, const char* tag, const char* format, Args... args) {
        if (level > getLevel())
          return;

        StringPrint buffer;

#if CONFIG_ARDUHAL_LOG_COLORS
        buffer.print(_colors[level]);
#endif

        // esp_timer_get_time in us, so shift left 10 to get ms (1024 vs 1000 won't hurt the logging feature)
        buffer.printf("%c %12u %-" MYCILA_LOGGER_TASK_NAME_LENGTH "." MYCILA_LOGGER_TASK_NAME_LENGTH "s (%u) %-" MYCILA_LOGGER_TAG_LENGTH "." MYCILA_LOGGER_TAG_LENGTH "s ", _codes[level], static_cast<uint32_t>(esp_timer_get_time() >> 10), pcTaskGetName(NULL), xPortGetCoreID(), tag);
        buffer.printf(format, args...);

#if CONFIG_ARDUHAL_LOG_COLORS
        buffer.print("\033[0m");
#endif

        buffer.print("\r\n");

        for (auto& _output : _outputs) {
          _output->print(buffer.str());
        }
      }

    private:
      std::vector<Print*> _outputs;
      const char* _codes = " EWID";
#if CONFIG_ARDUHAL_LOG_COLORS
      const char* _colors[5] = {"", ARDUHAL_LOG_COLOR_E, ARDUHAL_LOG_COLOR_W, ARDUHAL_LOG_COLOR_I, ARDUHAL_LOG_COLOR_D};
#endif
  };

  extern LoggerClass Logger;
} // namespace Mycila
