#include "BootTries.h"

int bootTries;

int getBootTries() {
    bootTries = -1;
    if (SPIFFS.exists("/boot_tries")) {
        File f = SPIFFS.open("/boot_tries", "r");
        if (f) {
            bootTries = f.readString().toInt();
            f.close();
            f = SPIFFS.open("/boot_tries", "w");
            if (f) {
                f.printf("%i", ++bootTries);
                f.flush(); f.close();
            }
            INFO("New boot script try #" + String(bootTries));
        }
    }
    return bootTries;
}

void BootTriesClass::begin() {
    if (getBootTries() <= 10) {
        if(!executeFile("/boot")) {
            INFO("No boot file - starting in AP mode");
            startAP(true);
        }
    } else { 
        INFO("Boot script disabled due to boot loop");
        bootTries = -1;
    }
}

void BootTriesClass::handle() {
    if (bootTries>=0 && millis()>10000) {
        SPIFFS.remove("/boot_tries");
        INFO("Commiting the boot script");
        bootTries = -1;
    }
}

BootTriesClass BootTries;
