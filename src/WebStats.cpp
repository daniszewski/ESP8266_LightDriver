#include "WebStats.h"
#include "PinDriver.h"

String getStats() {
    String result = "{\n";
    result += "\"name\": \"" + getDriverName() + "\",\n";
    result += "\"freemem\": "+String(system_get_free_heap_size()) + ",\n";
    result += "\"admin\": "+String(isAdmin()) + ",\n";
    result += "\"pins\": [\n";
    bool second = false;
    for(int i=0;i<PINCOUNTALL;i++) {
        uint8_t pin = PinDriver.getPin(i);
        char type = PinDriver.getPinType(pin);
        if (type < 'A') continue;
        String name = "D_";
        if (i>9) name.setCharAt(0, 'V');
        name.setCharAt(1, '0'+i);
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
        if (second) result += ",\n";
        else second = true;
        result += "{\"name\": \"" + name + "\", \"type\": \"" + typeName + "\", \"value\": " + value + " }";
    }
    result += "\n],\n\"WiFiClient_SSID\": \"" + WiFi.SSID() + "\",\n";
    result += "\"WiFiClient_IP\": \"" + WiFi.localIP().toString() + "\",\n";
    result += "\"WiFiAP_SSID\": \"" + WiFi.softAPSSID() + "\",\n";
    result += "\"WiFiAP_IP\": \"" + WiFi.softAPIP().toString() + "\",\n";
    result += "\"version\": \"" + getVersion() + "\"\n";
    result += "}\n";
    return result;
}