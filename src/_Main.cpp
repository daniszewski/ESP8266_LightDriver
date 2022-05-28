#include "Global.h"
#include "CrashToFS.h"

#include "LocalStorage.h"
#include "WebPages.h"
#include "WebCallsQueue.h"
#include "PowerAnimator.h"
#include "PinDriver.h"
#include "BootTries.h"
#include "WiFiAutoSwitch.h"
#include "MQTTMessages.h"
#include "I2C.h"
#include "Pinger.h"

void setup() {
    bootStart();

    LocalStorage.begin();
    WebPages.begin();
    WebCallsQueue.begin();
    PowerAnimator.begin();
    PinDriver.begin();
    BootTries.begin();
    WiFiAutoSwitch.begin();
    MQTTMessages.begin();
    I2C.begin();
    Pinger.begin();

    bootComplete();
}

void loop() {

    LocalStorage.handle();
    WebPages.handle();
    WebCallsQueue.handle();
    PowerAnimator.handle();
    PinDriver.handle();
    BootTries.handle();
    WiFiAutoSwitch.handle();
    MQTTMessages.handle();
    I2C.handle();
    Pinger.handle();

    delay(2); // leave some time for other handling
}
