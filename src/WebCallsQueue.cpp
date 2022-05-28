#include "WebCallsQueue.h"
#include <queue>
#include "Hardware.h"

#define MAX_QUEUE_SIZE 100

std::queue<String> urlsToPing;
WiFiClient client;

void WebCallsQueueClass::begin() {
    queueSize = 0;
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
    INFO("GET http://%s/%s\n", host.c_str(), path.c_str());
    if (!client.connect(host, 80) ) {
        INFO("Could not connect to %s\n", host.c_str());
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
    if (urlsToPing.empty()) return;
    callUrl(urlsToPing.front());
    urlsToPing.pop();
    queueSize--;
}

void WebCallsQueueClass::add(const String url) {
    if (queueSize < MAX_QUEUE_SIZE) {
        urlsToPing.push(url);
        queueSize++;
    } else {
        INFO("Queue is full\n");
    }
}

WebCallsQueueClass WebCallsQueue;
