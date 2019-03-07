#include "LocalStorage.h"

#define FORMAT_INDICATOR "/mac"
#define FIRMWARE "/www/firmware.bin"

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

    // firmware upgrade
    if (SPIFFS.exists(FIRMWARE)) {
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace, U_FLASH)) { //start with max available size
            Update.printError(Serial);
            Serial.println("ERROR");
        } else {
            File f = SPIFFS.open(FIRMWARE, "r");
            while (f.available()) {
                uint8_t ibuffer[128];
                f.read((uint8_t *)ibuffer, 128);
                Serial.println((char *)ibuffer);
                Update.write(ibuffer, sizeof(ibuffer));
            }
            Serial.print(Update.end(true));
            f.close();
        }
        SPIFFS.remove(FIRMWARE);
    }
}

LocalStorageClass LocalStorage;
