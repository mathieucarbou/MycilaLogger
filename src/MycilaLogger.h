// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2025 Mathieu Carbou
 */
#pragma once

#include <Arduino.h>
#include <StreamString.h>
#include <esp32-hal-log.h>
#include <vector>

#define MYCILA_LOGGER_VERSION "3.2.1"
#define MYCILA_LOGGER_VERSION_MAJOR 3
#define MYCILA_LOGGER_VERSION_MINOR 2
#define MYCILA_LOGGER_VERSION_REVISION 1

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
        buffer.print(_COLORS[level]);
#endif

        buffer.printf("[%6" PRIu32 "][%c][%s][%d][%s] ",
                      static_cast<uint32_t>(esp_timer_get_time() >> 10),
                      _CODES[level],
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

    public:
      /**
       * @brief Redirects the Arduino logs to this logger
       * @param logger The logger to redirect the logs to
       **/
      static void redirectArduinoLogs(Logger& logger); // NOLINT

      void redirectArduinoLogs() { redirectArduinoLogs(*this); }

    private:
      std::vector<Print*> _outputs;
#ifndef MYCILA_LOGGER_CUSTOM_LEVEL
      uint32_t _level = ARDUHAL_LOG_LEVEL;
#endif

    private:
      // redirection
      static StreamString* _arduinoLogBuffer;
      static Logger* _arduinoLogDestination;
      // static callback
      static void log_char(char c);
      // constants
      static char _CODES[5];
#if CONFIG_ARDUHAL_LOG_COLORS
      static const char* _COLORS[5];
#endif
  };
} // namespace Mycila
