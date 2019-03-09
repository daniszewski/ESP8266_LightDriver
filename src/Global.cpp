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

void firstBoot() {
    startAP(true);
}

void connectWifi(String ssid, String password)
{ 
    char _ssid[64];
    char _password[128]; 
    ssid.toCharArray(_ssid, 64); 
    password.toCharArray(_password, 128);    
    WiFi.begin(_ssid, _password); 
}

void startAP(bool resetWifi)
{ 
    if (resetWifi) {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
    }
    
    IPAddress localIp(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnet(255,255,255,0);
    
    WiFi.softAPConfig(localIp, gateway, subnet);
    char mac[32];
    (AP_prefix + WiFi.macAddress()).toCharArray(mac, 32, 0);
    WiFi.softAP(mac, wifiappwd, 10, 0, 4);
}

void stopAP() {
    WiFi.softAPdisconnect(false); 
    WiFi.enableAP(false);
}

void setAdminPassword(String pwd) {
    adminpwd = pwd;
}

void bootComplete() {
    _boot = false;
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
        return true;
    }
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
