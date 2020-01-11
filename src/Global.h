#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <Stream.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include "WiFiAutoSwitch.h"

int logToSerial(String msg);
String getLastError();
void setLastError(String error);
String getVersion();
String getScriptsPath();
String getWord(const String &line, int ix);
unsigned int parseTime(const String &time);
void WiFiSTA(String ssid, String password, String channel, bool persistent);
void WiFiAdd(String ssid, String password);
void WiFiAP(bool enable, bool persistent);
void setAdminPassword(String pwd);
bool isBoot();
bool isAdmin();
void setAdmin();
bool login(String password);
void logout();
void setDriverName(String error);
String getDriverName();
bool executeFile(String filename);
void bootStart();
void firstBoot();
void bootComplete();
void deleteFile(String filename);
void crash();

#ifndef NDEBUG
    #define ERR(...) { logToSerial(F("ERROR: ")); logToSerial( __VA_ARGS__ ); logToSerial(F("\n")); setLastError(__VA_ARGS__); }
    #define INFO(...) { logToSerial(F("INFO: ")); logToSerial( __VA_ARGS__ ); logToSerial(F("\n")); }
#else
    #define ERR(...) { setLastError(__VA_ARGS__); }
    #define INFO(...)
#endif

extern const char CHR_0;
extern const char CHR_9;

#endif
