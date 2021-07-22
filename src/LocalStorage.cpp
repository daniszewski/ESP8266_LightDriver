#include "LocalStorage.h"
#include "Global.h"

#define FORMAT_INDICATOR F("/boot")
#define FORMAT_INDICATOR2 F("/www/index.html")

void LocalStorageClass::begin() {
    LittleFS.begin();

    // file system format if required
    if (!LittleFS.exists(FORMAT_INDICATOR)) {
        bool format = !LittleFS.exists(FORMAT_INDICATOR2);
        if (format) Serial.println(F("FS: formatting"));
        if (!format || LittleFS.format()) {
            File f = LittleFS.open(FORMAT_INDICATOR, "w");
            if (f) {
                Serial.println(F("FS: formatting done"));
                f.println(PSTR("NAME ") + WiFi.macAddress());
                f.close();
                firstBoot();
            }
            else Serial.println(F("FS: formatting error (creating indicator file)"));
        } else Serial.println(F("FS: formatting error"));
    }
}

LocalStorageClass LocalStorage;
