#include "BootTries.h"

int bootTries;

static const char * BOOT_TRIES = "/boot_tries";

int getBootTries() {
    bootTries = -1;
    if (LittleFS.exists(BOOT_TRIES)) {
        File f = LittleFS.open(BOOT_TRIES, "r");
        if (f) {
            bootTries = f.readString().toInt();
            f.close();
            f = LittleFS.open(BOOT_TRIES, "w");
            if (f) {
                f.printf("%i", ++bootTries);
                f.flush(); f.close();
            }
            INFO("New boot script try #%d\n", bootTries);
        }
    }
    return bootTries;
}

void BootTriesClass::begin() {
    if (getBootTries() <= 10) {
        executeFile("/boot");
    } else { 
        INFO("Boot script disabled due to boot loop\n");
        bootTries = -1;
    }
}

void BootTriesClass::handle() {
    if (bootTries>=0 && millis()>10000) {
        LittleFS.remove(BOOT_TRIES);
        INFO("Commiting the boot script\n");
        bootTries = -1;
    }
}

BootTriesClass BootTries;
