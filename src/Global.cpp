#include "Global.h"
#include "Commands.h"
#include "_Config.h"
#include "DebugWiFi.h"
#include "MQTTMessages.h"

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


unsigned int parseTime(const String &time) {
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
    DebugWifi;
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);   
    Serial.begin(74880); //115200
}

void firstBoot() {
    WiFiAP(true, true);
}

void bootComplete() {
    _boot = false;
    INFO("WiFi mode: %d\n", WiFi.getMode());
}

void WiFiSTA(String ssid, String password, String channel, bool persistent) {
    WiFi.persistent(persistent);
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
}

void WiFiAdd(String ssid, String password) {
    WiFiAutoSwitch.add(ssid.c_str(), password.c_str());
}

void WiFiAP(bool enable, bool persistent) {
    WiFi.persistent(persistent);
    if (enable) {
        const char * mac = (AP_prefix + WiFi.macAddress()).c_str();
        if (WiFi.softAP(mac, wifiappwd, 6, false, 4)) { 
            INFO("AP started\n");
            delay(100);
            IPAddress localIp(192,168,4,1);
            IPAddress gateway(192,168,4,1);
            IPAddress subnet(255,255,255,0);
            if (WiFi.softAPConfig(localIp, gateway, subnet)) {
                INFO("Soft-AP with IP: %s\n", WiFi.softAPIP().toString().c_str());
            } else { INFO("AP config error\n"); }
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

int executeFile(String filename) {
    File f = SPIFFS.open(filename, "r");
    if (f) { 
        int line = 1;
        while (f.available()) {
            if (!execute(f.readStringUntil('\n'))) {
                INFO("%s: unknown command in line %d\n", filename.c_str(), line);
                f.close();
                return line;
            }
            line++;
        }
        f.close(); 
        return 0; 
    }
    INFO("%s: file doesn't exist.\n", filename.c_str());
    return -1;
}

void deleteFile(String filename) {
    if (isAdmin()) SPIFFS.remove(filename);
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