#include "Global.h"
#include "LocalStorage.h"
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
    bootComplete();
}

void loop() {
    WebPages.handle();
    WebCallsQueue.handle();
    LightAnimator.handle();
    PinDriver.handle();
    BootTries.handle();
    delay(2);
}
