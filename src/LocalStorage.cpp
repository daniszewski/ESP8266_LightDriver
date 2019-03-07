#include "LocalStorage.h"

#define FORMAT_INDICATOR "/mac"

void LocalStorageClass::begin() {
    SPIFFS.begin();

    // file system format
    if (!SPIFFS.exists(FORMAT_INDICATOR)) {
        Serial.println("SPIFFS: formatting");
        if (SPIFFS.format()) {
            File f = SPIFFS.open(FORMAT_INDICATOR, "w");
            if (f) {
                f.println(WiFi.macAddress());
                f.close();
            }
            else Serial.println("SPIFFS: formatting error (creating indicator file)");
        } else Serial.println("SPIFFS: formatting error");
    }
}

LocalStorageClass LocalStorage;
