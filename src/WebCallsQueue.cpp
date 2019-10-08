#include "WebCallsQueue.h"
#include <ESP8266HTTPClient.h>

//TODO: get rid of ESP8266HTTPClient to save memory (5kB progmem and 300b ram)

String urlsToPing = "";
WiFiClient client;
HTTPClient http;

void WebCallsQueueClass::begin() {
}

void WebCallsQueueClass::handle() {
    if (!WiFi.isConnected()) return;
    if (!urlsToPing.length()) return;
    int nextSpace = urlsToPing.indexOf(' ', 1);
    String url = nextSpace < 0 ? urlsToPing : urlsToPing.substring(0, nextSpace);
    int len = url.length();
    url.trim();
    int httpCode = 1;
    if (url.length()>2) { 
        INFO(PSTR("Calling url: ") + url);
        http.begin(client, url);
        httpCode = http.GET();
        INFO(PSTR("Result: ") + String(httpCode));
        http.end();
    }
    if(httpCode > 0) urlsToPing = urlsToPing.substring(len);
}

void WebCallsQueueClass::add(String url) {
    urlsToPing += " " + url;
}

WebCallsQueueClass WebCallsQueue;
