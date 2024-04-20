// SPDX-License-Identifier: MIT
/*
 * Copyright (C) 2023-2024 Mathieu Carbou and others
 */
#pragma once

#include <Arduino.h>
#include <Print.h>
#include <WString.h>
#include <esp32-hal-log.h>
#include <vector>

#define MYCILA_LOGGER_VERSION "2.2.1"
#define MYCILA_LOGGER_VERSION_MAJOR 2
#define MYCILA_LOGGER_VERSION_MINOR 2
#define MYCILA_LOGGER_VERSION_REVISION 1

#define MYCILA_LOGGER_TASK_NAME_LENGTH "10"
#define MYCILA_LOGGER_TAG_LENGTH "10"

namespace Mycila {
  class LoggerBuffer : public Print {
    public:
      size_t write(const uint8_t* p, size_t n) override { return _buffer.concat(reinterpret_cast<const char*>(p), n) ? n : 0; }
      size_t write(uint8_t c) override { return _buffer.concat(static_cast<char>(c)) ? 1 : 0; }
      const String& buffer() const { return _buffer; }

    private:
      String _buffer;
  };

  class LoggerClass {
    public:
#ifdef MYCILA_LOGGER_CUSTOM_LEVEL
      // To be implemented by the user.
      // Returns the current logging level, which could be sourced from a config system or a macro
      uint8_t getLevel() const;
#else
      uint8_t getLevel() const { return _level; }
      void setLevel(uint8_t level) { _level = level; }
#endif

    public:
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

        LoggerBuffer buffer;

#if CONFIG_ARDUHAL_LOG_COLORS
        buffer.print(_colors[level]);
#endif

        // esp_timer_get_time in us, so shift left 10 to get ms (1024 vs 1000 won't hurt the logging feature)
#if ESP_IDF_VERSION_MAJOR >= 5
        buffer.printf("%c %12lu %-" MYCILA_LOGGER_TASK_NAME_LENGTH "." MYCILA_LOGGER_TASK_NAME_LENGTH "s (%u) %-" MYCILA_LOGGER_TAG_LENGTH "." MYCILA_LOGGER_TAG_LENGTH "s ", _codes[level], static_cast<uint32_t>(esp_timer_get_time() >> 10), pcTaskGetName(NULL), xPortGetCoreID(), tag);
#else
        buffer.printf("%c %12u %-" MYCILA_LOGGER_TASK_NAME_LENGTH "." MYCILA_LOGGER_TASK_NAME_LENGTH "s (%u) %-" MYCILA_LOGGER_TAG_LENGTH "." MYCILA_LOGGER_TAG_LENGTH "s ", _codes[level], static_cast<uint32_t>(esp_timer_get_time() >> 10), pcTaskGetName(NULL), xPortGetCoreID(), tag);
#endif
        buffer.printf(format, args...);

#if CONFIG_ARDUHAL_LOG_COLORS
        buffer.print("\033[0m");
#endif

        buffer.print("\r\n");

        for (auto& output : _outputs)
          output->print(buffer.buffer());
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

  extern LoggerClass Logger;
} // namespace Mycila
