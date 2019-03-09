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
void connectWifi(String ssid, String password);
void startAP(bool resetWifi);
void stopAP();
void setAdminPassword(String pwd);
void bootComplete();
bool isBoot();
bool isAdmin();
void setAdmin();
bool login(String password);
void logout();
void setDriverName(String error);
String getDriverName();
bool executeFile(String filename);
void firstBoot();


#define ERR(...) { logToSerial("ERROR: "); logToSerial( __VA_ARGS__ ); logToSerial("\n"); setLastError(__VA_ARGS__); }
#define INFO(...) { logToSerial("INFO: "); logToSerial( __VA_ARGS__ ); logToSerial("\n"); }

#endif
