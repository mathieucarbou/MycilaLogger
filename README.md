# MycilaLogger

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Continuous Integration](https://github.com/mathieucarbou/MycilaLogger/actions/workflows/ci.yml/badge.svg)](https://github.com/mathieucarbou/MycilaLogger/actions/workflows/ci.yml)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/mathieucarbou/library/MycilaLogger.svg)](https://registry.platformio.org/libraries/mathieucarbou/MycilaLogger)

A simple and efficient logging library for Arduino / ESP32.

## Usage

### Implement the `getLevel()` method

This method must return the current logging level used in your app.

Example by re-using `ARDUHAL_LOG_LEVEL`:

```c++
uint8_t Mycila::LoggerClass::getLevel() const { return ARDUHAL_LOG_LEVEL; }
```

Example by using a configuration system:

```c++
uint8_t Mycila::LoggerClass::getLevel() const {
  return Mycila::Config.getBool(KEY_DEBUG_ENABLE) ? ARDUHAL_LOG_LEVEL_DEBUG : ARDUHAL_LOG_LEVEL_INFO;
}
```

### Use the logging API

First setup the outputs where to forward the logs in the `setup()` method:

```c++
Serial.begin(115200);
Mycila::Logger.forwardTo(&Serial);

WebSerial.begin(...);
Mycila::Logger.forwardTo(&Serial);
```

Then use the logger:

```c++
Mycila::Logger.debug(TAG, "Published in %u ms", (millis() - start));

if(Mycila::Logger.isDebugEnabled()) {
  // some expensive debug code
}
```

will output something like this:

```
D      8102600 loopTask   (1)  WEBSITE Published in 38 ms
```

- `loopTask` is the task name
- `(1)` is the core ID (usually 0 or 1)
- `WEBSITE` is the tag

## Configuration

- Colors are supported with `-D CONFIG_ARDUHAL_LOG_COLORS`
- You have access to a few define for feature detection: `MYCILA_LOGGING_VERSION`

## Thread safety

The logging code itself is thread-safe and can be called from multiple tasks and cores.

There is no locking strategy in place: the logging methods are delegating to the underlying `Print` implementations.
