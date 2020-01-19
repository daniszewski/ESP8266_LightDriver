#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <Stream.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include "WiFiAutoSwitch.h"

String formatString(PGM_P format, ...);
int logToSerial(String msg);
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
int executeFile(String filename);
void bootStart();
void firstBoot();
void bootComplete();
void deleteFile(String filename);
void crash();
void mqttInit(String server, String port, String user, String passwd);
const String mqttGetPrefix();
void mqttSetPrefix(String prefix);
void mqttSubscribe(String topic);
void mqttMessage(String topic, String message);
void setTime(String secsSince1970);

#ifdef PROJECT_DEBUG
    #define INFO(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ##__VA_ARGS__ )
#else
    #define INFO(...)
#endif

extern const char CHR_0;
extern const char CHR_9;

#endif
