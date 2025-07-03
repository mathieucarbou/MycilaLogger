#include <Arduino.h>
#include <MycilaLogger.h>
#include <StreamString.h>

Mycila::Logger logger;
StreamString logBuffer;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    continue;

  Mycila::Logger::redirectArduinoLogs(logger);

  logger.setLevel(ARDUHAL_LOG_LEVEL_DEBUG);

  logger.forwardTo(&logBuffer);

  log_d("Arduino debug message");
  log_i("Arduino info message");
  log_w("Arduino warning message");
  log_e("Arduino error message");

  Serial.println("Captured Arduino logs:");
  Serial.println(logBuffer);
  logBuffer.clear();

  logger.debug("APP", "A debug message");
  logger.info("APP", "An info message");
  logger.warn("APP", "A warning message");
  logger.error("APP", "An error message");

  Serial.println("Captured Mycila logger logs:");
  Serial.println(logBuffer);
  logBuffer.clear();
}

void loop() {
  delay(500);
}
