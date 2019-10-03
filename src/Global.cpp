#include "Global.h"
#include "Commands.h"
#include "_Config.h"

extern "C" {
    #include "user_interface.h"
}

String lastError;
String driverName;
bool _admin = false;
bool _boot = true;

const char CHR_0 = '0';
const char CHR_9 = '9';

int logToSerial(String msg) { 
    Serial.print(msg); 
    return 0; 
}

void setLastError(String error) {
    lastError = error;
}

String getLastError() {
    return lastError;
}

int getFreeMem() {
    return system_get_free_heap_size();
}

String getVersion() {
    return version;
}

String getScriptsPath() {
    return scriptsPath;
}

String getWord(String &line, int ix) {
    int p1 = 0;
    while (ix-- && p1 >= 0) p1 = line.indexOf(' ', p1+1);
    if (p1<0) return String();
    if (p1>0) p1++;
    int p2 = line.indexOf(' ', p1);
    if (p2<0) p2 = line.length();
    String r = line.substring(p1,p2);
    r.trim();
    return r;
}


unsigned int parseTime(String &time) {
    unsigned int result = 0;
    unsigned short part = 0;
    unsigned short len = time.length();
    bool dot = false;
    for (unsigned short i=0; i < len; i++)
    {
        char c = time[i];
        if (c >= CHR_0 && c <= CHR_9) part = (ushort)(part * 10 + (c - CHR_0));
        else if (c == ':')
        {
            result = (result + part) * 60;
            part = 0;
        }
        else if (c == '.')
        {
            result = result + part;
            dot = true;
            part = 0;
            if (len - i == 2) part = (ushort)((time[++i] - CHR_0) * 10);
            else if (len - i == 3) part = (ushort)((time[i + 1] - CHR_0) * 10 + (time[i + 2] - CHR_0));
            break;
        }
    }
    if (!dot) result = (result + part) * 100;
    else result = result * 100 + part;
    return result;
}

void bootStart() {
    Serial.begin(74880); //115200
}

void firstBoot() {
    WiFiAP(true, true);

}

void bootComplete() {
    _boot = false;
    INFO("WiFi mode: " + String(WiFi.getMode()));
}

void WiFiSTA(String ssid, String password, bool persistent) {
    if (ssid.length()>0) {
        WiFi.persistent(persistent);
        char _ssid[64];
        char _password[128]; 
        ssid.toCharArray(_ssid, 64); 
        password.toCharArray(_password, 128);
        WiFi.begin(_ssid, _password);
    } else {
        WiFi.enableSTA(false);
    }
}

void WiFiAP(bool enable, bool persistent) {
    WiFi.persistent(persistent);
    if (enable) {
        const char * mac = (AP_prefix + WiFi.macAddress()).c_str();
        if (WiFi.softAP(mac, wifiappwd, 10, false, 4)) { 
            IPAddress localIp(192,168,4,1);
            IPAddress gateway(192,168,4,1);
            IPAddress subnet(255,255,255,0);
            delay(100);
            if (WiFi.softAPConfig(localIp, gateway, subnet)) {
                INFO(F("AP started"));
                Serial.print(F("Soft-AP with IP: "));
                Serial.println(WiFi.softAPIP());
            } else { ERR(F("AP config error")); }
        } else { ERR(F("AP start error")); }
    } else {
        WiFi.enableAP(false);
    }
}

void setAdminPassword(String pwd) {
    adminpwd = pwd;
}

bool isBoot() {
    return _boot;
}

bool isAdmin() {
    return _admin || _boot;
}

void setAdmin() {
    if(_boot) _admin = true;
}

bool login(String password) {
    if (adminpwd == password) {
        _admin = true;
        INFO(F("Logged in"));
        return true;
    }
    ERR(F("Wrong password"));
    return false;
}

void logout() {
    _admin = false;
}

void setDriverName(String name) {
    driverName = name;
}

String getDriverName() {
    return driverName;
}

bool executeFile(String filename) {
    File f = SPIFFS.open(filename, "r");
    if (f) { 
        int line = 1;
        while (f.available()) {
            if (!execute(f.readStringUntil('\n'))) {
                ERR(filename+": unknown command in line "+String(line));
                f.close();
            }
            line++;
        }
        f.close(); 
        return true; 
    }
    ERR(filename+": file doesn't exist.");
    return false;
}

void deleteFile(String filename) {
    if (isAdmin()) SPIFFS.remove(filename);
}