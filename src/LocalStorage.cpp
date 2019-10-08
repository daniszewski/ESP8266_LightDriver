#include "LocalStorage.h"
#include "Global.h"

#define FORMAT_INDICATOR F("/boot")
#define FORMAT_INDICATOR2 F("/www/index.html")

void LocalStorageClass::begin() {
    SPIFFS.begin();

    // file system format if required
    if (!SPIFFS.exists(FORMAT_INDICATOR)) {
        bool format = !SPIFFS.exists(FORMAT_INDICATOR2);
        if (format) Serial.println(F("SPIFFS: formatting"));
        if (!format || SPIFFS.format()) {
            File f = SPIFFS.open(FORMAT_INDICATOR, "w");
            if (f) {
                Serial.println(F("SPIFFS: formatting done"));
                f.println(PSTR("NAME ") + WiFi.macAddress());
                f.close();
                firstBoot();
            }
            else Serial.println(F("SPIFFS: formatting error (creating indicator file)"));
        } else Serial.println(F("SPIFFS: formatting error"));
    }
}

LocalStorageClass LocalStorage;
