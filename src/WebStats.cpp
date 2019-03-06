#include "WebStats.h"
#include "PinDriver.h"

String getStats() {
    String result = "{\n";
    result += "\"name\": \"" + getDriverName() + "\",\n";
    result += "\"freemem\": "+String(system_get_free_heap_size()) + ",\n";
    result += "\"admin\": "+String(isAdmin()) + ",\n";
    for(int i=0;i<PINCOUNTALL;i++) {
        uint8_t pin = PinDriver.getPin(i);
        char type = PinDriver.getPinType(pin);
        if (type < 'A') continue;
        String name = "D_";
        if (i>9) name.setCharAt(0, 'V');
        name.setCharAt(1, '0'+i);
        String value;
        String typeName;
        if (type=='S') {
            typeName = "SWITCH";
            value = String(PinDriver.getSwitchValue(pin) ? 1 : 0);
        } else if (type=='P' || type=='Z') {
            typeName = "LIGHT";
            value = String(PinDriver.getPinAnim(pin)->getValue());
        }
        result += "\""+name+"\": { \"type\": \""+typeName+"\", \"value\": "+value+" },\n";
    }
    result += "\"version\": \"" + getVersion() + "\"\n";
    result += "}\n";
    return result;
}