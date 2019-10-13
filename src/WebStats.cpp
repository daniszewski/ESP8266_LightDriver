#include "WebStats.h"
#include "PinDriver.h"

void sendStats(ESP8266WebServer *server) {
    server->sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
    server->sendHeader(F("Pragma"), F("no-cache"));
    server->sendHeader(F("Expires"), F("-1"));
    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    server->send(200, F("application/json"), F("{\n\"name\": \""));
    server->sendContent(getDriverName());
    server->sendContent(F("\",\n\"freemem\": "));
    server->sendContent(String(system_get_free_heap_size()));
    server->sendContent(F(",\n\"admin\": "));
    server->sendContent(String(isAdmin()));
    server->sendContent(F(",\n\"pins\": [\n"));

    bool second = false;
    for(int i=0;i<PINCOUNTALL;i++) {
        uint8_t pin = PinDriver.getPin(i);
        char type = PinDriver.getPinType(pin);
        if (type < 'A') continue;
        String name = "D_";
        if (i>9) name.setCharAt(0, 'V');
        name.setCharAt(1, CHR_0 + (i > 9 ? i - 10 : i));
        String value;
        String typeName;
        if (type=='X') {
            typeName = "ZERO";
            value = String(PinDriver.getZeroFreq());
        } else if (type=='S') {
            typeName = "SWITCH";
            value = String(PinDriver.getSwitchValue(pin) ? 1 : 0);
        } else if (type=='P' || type=='Z') {
            typeName = type=='P' ? "PWM" : "PHASE";
            PowerAnimation* anim = PinDriver.getPinAnim(pin);
            value = String(anim->getValue()) + ", \"seqstep\": " + String(anim->getStepIndex()) + ", \"timeleft\": " + String(anim->getStepTime());
        } else if (type=='O') {
            typeName = "ON/OFF";
            PowerAnimation* anim = PinDriver.getPinAnim(pin);
            value = String(anim->getValue() < 512 ? "\"ON\"" : "\"OFF\"") + ", \"seqstep\": " + String(anim->getStepIndex()) + ", \"timeleft\": " + String(anim->getStepTime());;
        }
        if (second) server->sendContent(",\n");
        else second = true;
        server->sendContent(F("{\"name\": \""));
        if (name) server->sendContent(name);
        else server->sendContent(F("unnamed"));
        server->sendContent(F("\", \"type\": \""));
        server->sendContent(typeName);
        server->sendContent(F("\", \"value\": "));
        server->sendContent(value);
        server->sendContent(F(" }"));
    }
    server->sendContent(F("\n],\n\"WiFiClient_SSID\": \""));
    server->sendContent(WiFi.SSID());
    server->sendContent(F("\",\n\"WiFiClient_IP\": \""));
    server->sendContent(WiFi.localIP().toString());
    server->sendContent(F("\",\n\"WiFiAP_SSID\": \""));
    server->sendContent(WiFi.softAPSSID());
    server->sendContent(F("\",\n\"WiFiAP_IP\": \""));
    server->sendContent(WiFi.softAPIP().toString());
    server->sendContent(F("\",\n\"version\": \""));
    server->sendContent(getVersion());
    server->sendContent(F("\"\n}\n"));
}