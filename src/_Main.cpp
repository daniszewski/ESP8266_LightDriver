#include "Global.h"
#include <ArduinoOTA.h>
#include "WebPages.h"
#include "LedAnimator.h"
#include "PinDriver.h"
#include "BootTries.h"
#include "WebCallsQueue.h"

void setup() {
  Serial.begin(74880); //115200
  SPIFFS.begin();
  LedAnimator.begin([](){ PinDriver.updatePinsPwm(); });
  BootTries.begin();
  PinDriver.begin();
  WebPages.begin();
  WebCallsQueue.begin();
  ArduinoOTA.begin();
  bootComplete();
}

void loop() {
  if (isAdmin()) ArduinoOTA.handle();
  WebPages.handle();
  WebCallsQueue.handle();
  LedAnimator.handle();
  PinDriver.handle();
  BootTries.handle();
  delay(2);
}


