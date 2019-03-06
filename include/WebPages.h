#ifndef WEBPAGES_H
#define WEBPAGES_H

#include "Global.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

class WebPagesClass {
  public:
    void begin();
    void handle();
  
  protected:
    int lastHttpTextOrigLength;
    void handleStaticPage();
    void handleJson(String json);
    void handleBoot();
    void handleRun();
    void handleDir();
    void handleFileUpload();
    String getContentType(String filename);
};

extern WebPagesClass WebPages;

#endif
