#ifndef WIFIAUTOSWITCH_H
#define WIFIAUTOSWITCH_H

#include <vector>

#ifdef NDEBUG
  #define DEBUG_WIFI_MULTI(...)
#else
  #define DEBUG_WIFI_MULTI(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ##__VA_ARGS__ )
#endif

#define WIFI_MIN_RSSI -80
#define WIFI_CHECK_EVERY 1000
#define WIFI_BAD_READS_TRIGGER 30

struct WiFiCredential {
	char * SSID;
	char * pwd;
};

typedef std::vector<WiFiCredential> WiFiCredentialList;

class WiFiAutoSwitchClass {
  private:
    WiFiCredentialList _wifilist;
    char _badreads;
    unsigned long lastTime;

  public:
    void begin();
    void handle();
    void add(const char* ssid, const char *passphrase);
    void findBetterAP();
};

extern WiFiAutoSwitchClass WiFiAutoSwitch;

#endif
