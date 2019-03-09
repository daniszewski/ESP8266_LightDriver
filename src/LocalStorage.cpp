#include "LocalStorage.h"
#include "Global.h"

#define FORMAT_INDICATOR "/boot"

void LocalStorageClass::begin() {
    SPIFFS.begin();

    // file system format
    if (!SPIFFS.exists(FORMAT_INDICATOR)) {
        Serial.println("SPIFFS: formatting");
        if (SPIFFS.format()) {
            File f = SPIFFS.open(FORMAT_INDICATOR, "w");
            if (f) {
                Serial.println("SPIFFS: formatting done");
                f.println("NAME " + WiFi.macAddress());
                f.close();
                firstBoot();
            }
            else Serial.println("SPIFFS: formatting error (creating indicator file)");
        } else Serial.println("SPIFFS: formatting error");
    }
}

LocalStorageClass LocalStorage;
