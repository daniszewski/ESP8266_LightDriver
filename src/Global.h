#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <Stream.h>
#include "LocalStorage.h"
#include "Hardware.h"

String formatString(PGM_P format, ...);
int logToSerial(String msg);
String& getVersion();
String& getScriptsPath();
String getWord(const String &line, int ix);
String getWord(const String &line, int ix, bool toEnd);
void WiFiSTA(String ssid, String password, String channel);
void WiFiAP(bool enable);
void setAdminPassword(String pwd);
bool isBoot();
bool isAdmin();
void setAdmin();
bool login(String password);
void logout();
void setDriverName(String name);
String& getDriverName();
String executeFile(String filename);
void bootStart();
void firstBoot();
void bootComplete();
void deleteFile(String filename);
void crash();
void mqttInit(String server, String port, String user, String passwd);
String& mqttGetPrefix();
void mqttSetPrefix(String prefix);
void mqttSubscribe(String topic);
void mqttMessage(String topic, String message);
void setTime(String secsSince1970);


extern const char CHR_0;
extern const char CHR_9;

#endif
