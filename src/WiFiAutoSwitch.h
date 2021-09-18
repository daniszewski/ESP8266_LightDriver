#ifndef WIFIAUTOSWITCH_H
#define WIFIAUTOSWITCH_H

#include <vector>

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
    void startScan();
    void scanComplete(int scanResult);
};

extern WiFiAutoSwitchClass WiFiAutoSwitch;

#endif
