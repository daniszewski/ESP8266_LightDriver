#include "Global.h"
#include "LocalStorage.h"
#include "WebPages.h"
#include "PowerAnimator.h"
#include "PinDriver.h"
#include "BootTries.h"
#include "WebCallsQueue.h"
#include "CrashToSpiffs.h"
#include "MQTTMessages.h"

void setup() {
    bootStart();
    LocalStorage.begin();
    PowerAnimator.begin([](){ PinDriver.updatePinsPwm(); });
    BootTries.begin();
    PinDriver.begin();
    WebPages.begin();
    WebCallsQueue.begin();
    WiFiAutoSwitch.begin();
    MQTTMessages.begin();
    bootComplete();
}

void loop() {
    WebPages.handle();
    WebCallsQueue.handle();
    PowerAnimator.handle();
    PinDriver.handle();
    BootTries.handle();
    WiFiAutoSwitch.handle();
    MQTTMessages.handle();
    delay(2);
}
