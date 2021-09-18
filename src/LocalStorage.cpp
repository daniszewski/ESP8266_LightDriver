#include <LittleFS.h>
#include "LocalStorage.h"
#include "Global.h"

#define FORMAT_INDICATOR F("/boot")
#define FORMAT_INDICATOR2 F("/www/index.html")

void LocalStorageClass::begin() {
    LittleFS.begin();

    // file system format if required
    if (!LittleFS.exists(FORMAT_INDICATOR)) {
        bool format = !LittleFS.exists(FORMAT_INDICATOR2);
        if (format) INFO("FS: formatting\n");
        if (!format || LittleFS.format()) {
            File f = LittleFS.open(FORMAT_INDICATOR, "w");
            if (f) {
                INFO("FS: formatting done\n");
                f.println(PSTR("NAME ") + WiFi.macAddress());
                f.close();
                firstBoot();
            }
            else INFO("FS: formatting error (creating indicator file)\n");
        } else INFO("FS: formatting error\n");
    }
}

void LocalStorageClass::handle() {
}

bool LocalStorageClass::exists(const char* path) {
    return LittleFS.exists(path);
}

File LocalStorageClass::open(const char* path, const char* mode) { return LittleFS.open(path, mode); }
File LocalStorageClass::open(const String& path, const char* mode) { return LittleFS.open(path, mode); }
Dir LocalStorageClass::openDir(const char* path) { return LittleFS.openDir(path); }
Dir LocalStorageClass::openDir(const String& path) { return LittleFS.openDir(path); }
bool LocalStorageClass::remove(const char* path) { return LittleFS.remove(path); }
bool LocalStorageClass::remove(const String& path) { return LittleFS.remove(path); }
bool LocalStorageClass::rename(const char* pathFrom, const char* pathTo) { return LittleFS.rename(pathFrom, pathTo); }
bool LocalStorageClass::rename(const char* pathFrom, const String& pathTo) { return LittleFS.rename(pathFrom, pathTo); }

LocalStorageClass LocalStorage;
