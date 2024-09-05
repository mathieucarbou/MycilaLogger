# MycilaLogger

[![Latest Release](https://img.shields.io/github/release/mathieucarbou/MycilaLogger.svg)](https://GitHub.com/mathieucarbou/MycilaLogger/releases/)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/mathieucarbou/library/MycilaLogger.svg)](https://registry.platformio.org/libraries/mathieucarbou/MycilaLogger)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](code_of_conduct.md)

[![Build](https://github.com/mathieucarbou/MycilaLogger/actions/workflows/ci.yml/badge.svg)](https://github.com/mathieucarbou/MycilaLogger/actions/workflows/ci.yml)
[![GitHub latest commit](https://badgen.net/github/last-commit/mathieucarbou/MycilaLogger)](https://GitHub.com/mathieucarbou/MycilaLogger/commit/)
[![Gitpod Ready-to-Code](https://img.shields.io/badge/Gitpod-Ready--to--Code-blue?logo=gitpod)](https://gitpod.io/#https://github.com/mathieucarbou/MycilaLogger)

A simple and efficient logging library for Arduino / ESP32.

## Basic Usage

First setup the outputs where to forward the logs in the `setup()` method:

```c++
Serial.begin(115200);
logger.forwardTo(&Serial);

WebSerial.begin(...);
logger.forwardTo(&Serial);
```

Then use the logger:

```c++
logger.debug(TAG, "Published in %u ms", (millis() - start));
logger.info(TAG, "Published in %u ms", (millis() - start));
logger.warn(TAG, "Published in %u ms", (millis() - start));
logger.error(TAG, "Published in %u ms", (millis() - start));

if(logger.isDebugEnabled()) {
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

Note: the logging level can be controlled with the standard Arduino flag: `-D CORE_DEBUG_LEVEL=ARDUHAL_LOG_LEVEL_DEBUG` for example.

## Tips

- Colors are supported with `-D CONFIG_ARDUHAL_LOG_COLORS`
- You have access to a few define for feature detection: `MYCILA_LOGGER_VERSION`

## Thread safety

The logging code itself is thread-safe and can be called from multiple tasks and cores.

There is no locking strategy in place: the logging methods are delegating to the underlying `Print` implementations.
