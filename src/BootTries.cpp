#include "BootTries.h"

int bootTries;

static const char * BOOT_TRIES = "/boot_tries";

int getBootTries() {
    bootTries = -1;
    if (SPIFFS.exists(BOOT_TRIES)) {
        File f = SPIFFS.open(BOOT_TRIES, "r");
        if (f) {
            bootTries = f.readString().toInt();
            f.close();
            f = SPIFFS.open(BOOT_TRIES, "w");
            if (f) {
                f.printf("%i", ++bootTries);
                f.flush(); f.close();
            }
            INFO(PSTR("New boot script try #") + String(bootTries));
        }
    }
    return bootTries;
}

void BootTriesClass::begin() {
    if (getBootTries() <= 10) {
        executeFile("/boot");
    } else { 
        INFO(F("Boot script disabled due to boot loop"));
        bootTries = -1;
    }
}

void BootTriesClass::handle() {
    if (bootTries>=0 && millis()>10000) {
        SPIFFS.remove(BOOT_TRIES);
        INFO(F("Commiting the boot script"));
        bootTries = -1;
    }
}

BootTriesClass BootTries;
