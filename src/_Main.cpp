#include "Global.h"
#include "LocalStorage.h"
#include <ArduinoOTA.h>
#include "WebPages.h"
#include "LightAnimator.h"
#include "PinDriver.h"
#include "BootTries.h"
#include "WebCallsQueue.h"

void setup() {
    bootStart();
    LocalStorage.begin();
    LightAnimator.begin([](){ PinDriver.updatePinsPwm(); });
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
    LightAnimator.handle();
    PinDriver.handle();
    BootTries.handle();
    delay(2);
}
