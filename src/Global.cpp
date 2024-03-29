#include "Global.h"
#include "Commands.h"
#include "_Config.h"
#include "DebugWiFi.h"
#include "MQTTMessages.h"
#include <time.h>

extern "C" {
    #include "user_interface.h"
}

String driverName;
bool _admin = false;
bool _boot = true;
String mqttPrefix;

const char CHR_0 = '0';
const char CHR_9 = '9';

int logToSerial(String msg) { 
    Serial.print(msg); 
    return 0; 
}

String formatString(PGM_P format, ...) {
    va_list arg;
    va_start(arg, format);
    char temp[64];
    char* buffer = temp;
    size_t len = vsnprintf_P(temp, sizeof(temp), format, arg);
    va_end(arg);
    String result;
    if (len > sizeof(temp) - 1) {
        buffer = new char[len + 1];
        if (!buffer) return result;
        va_start(arg, format);
        vsnprintf_P(buffer, len + 1, format, arg);
        va_end(arg);
    }
    result = String(buffer);
    if (buffer != temp) delete[] buffer;
    return result;
}

String& getVersion() {
    return version;
}

String& getScriptsPath() {
    return scriptsPath;
}

String getWord(const String &line, int ix) {
    return getWord(line, ix, false);
}

String getWord(const String &line, int ix, bool toEnd) {
    int p1 = 0;
    while (ix-- && p1 >= 0) p1 = line.indexOf(' ', p1+1);
    String r;
    if (p1<0) return r;
    if (p1>0) p1++;
    int p2 = toEnd ? -1 : line.indexOf(' ', p1);
    if (p2<0) p2 = line.length();
    r = line.substring(p1,p2);
    r.trim();
    return r;
}

void bootStart() {
    Serial.begin(74880); //115200
    INFO("Booting\n");
    DebugWifi;
}

void firstBoot() {
    WiFiAP(true);
}

void bootComplete() {
    _boot = false;
    INFO("WiFi mode: %d\n", WiFi.getMode());
    if (WiFi.getMode() == 0) {
        WiFiAP(true);
    }
}

void WiFiSTA(String ssid, String password, String channel) {
    WiFi.persistent(false);
    if (ssid.length()>0) {
        char _ssid[32];
        char _password[32]; 
        ssid.toCharArray(_ssid, 32); 
        password.toCharArray(_password, 32);
        if(!channel.length()) {
            WiFi.begin(_ssid, _password);
        } else {
            WiFi.begin(_ssid, _password, channel.toInt());
        }
    } else {
        WiFi.enableSTA(false);
    }
    WiFi.setAutoReconnect(true);
}

void WiFiAP(bool enable) {
    WiFi.persistent(false);
    if (enable) {
        const char * ssid = "ESPAP";//(AP_prefix + WiFi.softAPmacAddress()).c_str();
        IPAddress localIp(192,168,4,1);
        IPAddress gateway(192,168,4,1);
        IPAddress subnet(255,255,255,0);
        if (WiFi.softAPConfig(localIp, gateway, subnet)) {
            INFO("Soft-AP with IP: %s\n", WiFi.softAPIP().toString().c_str());
        } else { INFO("AP config error\n"); }
        if (WiFi.softAP(ssid, wifiappwd, AP_port)) { 
            INFO("AP started\n");
        } else { INFO("AP start error\n"); }
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
        INFO("Logged in\n");
        return true;
    }
    INFO("Wrong password\n");
    return false;
}

void logout() {
    _admin = false;
}

void setDriverName(String name) {
    driverName = name;
}

String& getDriverName() {
    return driverName;
}

String executeFile(String filename) {
    File f = LocalStorage.open(filename, "r");
    String result;
    if (f) { 
        int line = 1;
        while (f.available()) {
            if ((result = execute(f.readStringUntil('\n'))).length()>0) {
                INFO("Script %s returned %s from line %d\n", filename.c_str(), result.c_str(), line);
                f.close();
                return result;
            }
            line++;
        }
        f.close(); 
        return "OK"; 
    }
    INFO("No script file: %s\n", filename.c_str());
    return "No file: " + filename;
}

void deleteFile(String filename) {
    if (isAdmin()) LocalStorage.remove(filename);
}

void crash() {
    int* nullPointer = NULL;
    Serial.print(*nullPointer);
}

void mqttInit(String server, String port, String user, String passwd) {
    MQTTMessages.setServer(server.c_str(), port.toInt(), user.c_str(), passwd.c_str());
}

void mqttSetPrefix(String prefix) {
    mqttPrefix = prefix;
}

String& mqttGetPrefix() {
    return mqttPrefix;
}

void mqttSubscribe(String topic) {
    MQTTMessages.subscribe(mqttPrefix + "/" + topic);
}

void mqttMessage(String topic, String message) {
    if (message.length() == 0) {
        MQTTMessages.sendMessage(mqttPrefix, topic);
    } else {
        MQTTMessages.sendMessage(mqttPrefix + "/" + topic, message);
    }
}

void setTime(String secsSince1970) {
    struct timeval tv = { .tv_sec = secsSince1970.toInt() };
    struct timezone tz = { .tz_minuteswest = 0, .tz_dsttime = 0 };
    settimeofday(&tv, &tz); // Syntax: TIME <seconds_since_1970>
}