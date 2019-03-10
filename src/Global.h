#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <Stream.h>
#include <FS.h>
#include <ESP8266WiFi.h>

int logToSerial(String msg);
String getLastError();
void setLastError(String error);
int getFreeMem();
String getVersion();
String getWord(String &line, int ix);
void WiFiSTA(String ssid, String password, bool persistent);
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


#define ERR(...) { logToSerial("ERROR: "); logToSerial( __VA_ARGS__ ); logToSerial("\n"); setLastError(__VA_ARGS__); }
#define INFO(...) { logToSerial("INFO: "); logToSerial( __VA_ARGS__ ); logToSerial("\n"); }

#endif
