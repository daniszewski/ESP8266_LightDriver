#ifndef H_HARDWARE

#define H_HARDWARE

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>

#define WebServer ESP8266WebServer

#ifndef NDEBUG
    #define INFO(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ##__VA_ARGS__ )
#else
    #define INFO(...)
#endif

#endif