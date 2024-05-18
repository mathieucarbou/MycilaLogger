#define MYCILA_LOGGER_CUSTOM_LEVEL

#include <MycilaLogger.h>

Mycila::Logger logger;

uint8_t Mycila::Logger::getLevel() const { return ARDUHAL_LOG_LEVEL_DEBUG; }

void setup() {
  Serial.begin(115200);
  while (!Serial)
    continue;

  logger.forwardTo(&Serial);
}

void loop() {
  logger.debug("APP", "A debug message");
  logger.info("APP", "An info message");
  logger.warn("APP", "A warning message");
  logger.error("APP", "An error message");
  delay(5000);
}
