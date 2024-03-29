#include "WebStats.h"
#include "PinDriver.h"
#include "MQTTMessages.h"
#include "time.h"

void sendStats(WebServer *server) {
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
        } else if (type=='O' || type=='P' || type=='Z') {
            typeName = type=='O' ? F("ON/OFF") : type=='P' ? F("PWM") : F("PHASE");
            PowerAnimation* anim = PinDriver.getPinAnim(pin);
            value = (type=='O' ? (anim->getValue() < 512 ? F("\"ON\"") : F("\"OFF\"")) : String(anim->getValue())) 
                + F(", \"seqstep\": ") + String(anim->getStepIndex()) + F(", \"timeleft\": ") + String(anim->getStepTime());
        }
        if (second) server->sendContent(F(",\n"));
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
    if(WiFi.SSID().length()>1) server->sendContent(WiFi.SSID());
    server->sendContent(F("\",\n\"WiFiClient_IP\": \""));
    server->sendContent(WiFi.localIP().toString());
    server->sendContent(F("\",\n\"WiFiClient_STATUS\": \""));
    server->sendContent(String(WiFi.status()));
    server->sendContent(F("\",\n\"WiFiClient_CHANNEL\": \""));
    server->sendContent(String(WiFi.channel()));
    server->sendContent(F("\",\n\"WiFiClient_RSSI\": \""));
    server->sendContent(String(WiFi.RSSI()));
    server->sendContent(F("\",\n\"WiFiAP_SSID\": \""));
    if(WiFi.softAPSSID().length()>1) server->sendContent(WiFi.softAPSSID());
    server->sendContent(F("\",\n\"WiFiAP_IP\": \""));
    server->sendContent(WiFi.softAPIP().toString());
    server->sendContent(F("\",\n\"MQTT_STATUS\": \""));
    server->sendContent(String(MQTTMessages.getStatus()));
    server->sendContent(F("\",\n\"time\": \""));
    server->sendContent(String(time(nullptr))); // seconds from 1st Jan 1970
    server->sendContent(F("\",\n\"version\": \""));
    server->sendContent(getVersion());
    server->sendContent(F("\"\n}\n"));
}