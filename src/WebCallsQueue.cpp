#include "WebCallsQueue.h"

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
        INFO("Calling url: " + url);
        http.begin(client, url);
        httpCode = http.GET();
        INFO("Result: " + String(httpCode));
        http.end();
    }
    if(httpCode > 0) urlsToPing = urlsToPing.substring(len);
}

void WebCallsQueueClass::add(String url) {
    urlsToPing += " " + url;
}

WebCallsQueueClass WebCallsQueue;
