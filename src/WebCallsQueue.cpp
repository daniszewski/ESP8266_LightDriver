#include "WebCallsQueue.h"

String urlsToPing = "";
WiFiClient client;

void WebCallsQueueClass::begin() {
}

String getWordExt(const String &line, int ix, char separator, bool isFinal) {
    int p1 = 0;
    while (ix-- && p1 >= 0) p1 = line.indexOf(separator, p1+1);
    if (p1<0) return String();
    if (p1>0) p1++;
    int p2 = isFinal ? -1 : line.indexOf(separator, p1);
    if (p2<0) p2 = line.length();
    String r = line.substring(p1,p2);
    r.trim();
    return r;
}

void callUrl(const String& url) {
    String host = getWordExt(url, 2, '/', false);
    String path = getWordExt(url, 3, '/', true);
    INFO("GET http://" + host + "/" + path);
    if (!client.connect(host, 80) ) {
        INFO("Could not connect to " +host);
        return;
    }

    client.println(PSTR("GET /")+path+PSTR(" HTTP/1.1"));
    client.print(F("Host: "));
    client.println(host);
    client.println(F("Connection: close"));
    client.println();
}

void WebCallsQueueClass::handle() {
    while (client.available()) client.read();

    if (!WiFi.isConnected()) return;
    if (!urlsToPing.length()) return;

    int nextSpace = urlsToPing.indexOf(' ', 1);
    String url = nextSpace < 0 ? urlsToPing : urlsToPing.substring(0, nextSpace);
    int len = url.length();
    url.trim();
    int httpCode = 1;
    if (url.length() > 2) callUrl(url);
    if(httpCode > 0) urlsToPing = urlsToPing.substring(len);
}

void WebCallsQueueClass::add(String url) {
    urlsToPing += " " + url;
}

WebCallsQueueClass WebCallsQueue;