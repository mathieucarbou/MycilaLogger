#include <MycilaLogger.h>

uint8_t Mycila::LoggerClass::getLevel() const { return ARDUHAL_LOG_LEVEL_DEBUG; }

void setup() {
  Serial.begin(9600);
  while (!Serial)
    continue;

  Mycila::Logger.forwardTo(&Serial);
}

void loop() {
  Mycila::Logger.debug("APP", "A debug message");  
  Mycila::Logger.info("APP", "An info message");  
  Mycila::Logger.warn("APP", "A warning message");  
  Mycila::Logger.error("APP", "An error message");  
}
