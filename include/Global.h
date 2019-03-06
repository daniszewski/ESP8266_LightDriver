#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <Stream.h>
#include <FS.h>
#include <ESP8266WiFi.h>

#define DEBUG 1

int logToSerial(String msg);
String getLastError();
void setLastError(String error);
int getFreeMem();
String getVersion();
String getWord(String &line, int ix);
void connectWifi(String ssid, String password);
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


#define ERR(...) { logToSerial("ERROR: "); logToSerial( __VA_ARGS__ ); logToSerial("\n"); setLastError(__VA_ARGS__); }

#ifdef DEBUG
  #define INFO(...) { logToSerial("INFO: "); logToSerial( __VA_ARGS__ ); logToSerial("\n"); }
#else
  #define INFO(...)
#endif

#endif
