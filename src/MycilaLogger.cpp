// SPDX-License-Identifier: MIT
/*
 * Copyright (C) Mathieu Carbou
 */
#include "MycilaLogger.h"

StreamString* Mycila::Logger::_arduinoLogBuffer = nullptr;
Mycila::Logger* Mycila::Logger::_arduinoLogDestination = nullptr;

char Mycila::Logger::_CODES[5] = {' ', 'E', 'W', 'I', 'D'};

#if CONFIG_ARDUHAL_LOG_COLORS
const char* Mycila::Logger::_COLORS[5] = {"", ARDUHAL_LOG_COLOR_E, ARDUHAL_LOG_COLOR_W, ARDUHAL_LOG_COLOR_I, ARDUHAL_LOG_COLOR_D};
#endif

void Mycila::Logger::log_char(char c) {
  if (!_arduinoLogDestination)
    return;
  _arduinoLogBuffer->write(c);
  if (c == '\n') {
    for (auto& output : _arduinoLogDestination->_outputs)
      output->print(*_arduinoLogBuffer);
    _arduinoLogBuffer->clear();
  }
}

void Mycila::Logger::redirectArduinoLogs(Logger& logger) {
  if (!_arduinoLogBuffer) {
    _arduinoLogBuffer = new StreamString();
    _arduinoLogBuffer->reserve(MYCILA_LOGGER_BUFFER_SIZE);
    // will override default arduino installed functions which sends logs to UART
    ets_install_putc1(log_char);
  }
  _arduinoLogBuffer->clear();
  _arduinoLogDestination = &logger;
}
