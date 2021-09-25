#include "WiFiAutoSwitch.h"
#include "Hardware.h"
#include <stdlib.h>

void WiFiAutoSwitchClass::begin() {
    lastTime = millis();
    _badreads = 0;
}

void WiFiAutoSwitchClass::handle() {
    if(WiFi.scanComplete() == WIFI_SCAN_RUNNING) return; // still scanning

    unsigned long currTime = millis();
    if (currTime - lastTime >= WIFI_CHECK_EVERY && _wifilist.size() > 0) {
        INFO("[MULTIWIFI] check\n");
        if (WiFi.status() != WL_CONNECTED || WiFi.RSSI() < WIFI_MIN_RSSI) {
            INFO("[MULTIWIFI] bad signal (status: %d, rssi: %d, counter: %d)\n", WiFi.status(), WiFi.RSSI(), _badreads);
            if (++_badreads > WIFI_BAD_READS_TRIGGER) {
                INFO("[MULTIWIFI] bad reads limit reached\n");
                startScan();
                _badreads = 0;
            }
        } else {
            _badreads = 0;
        }
        lastTime = currTime;
    }
}

void WiFiAutoSwitchClass::startScan() {
    INFO("[MULTIWIFI] looking for better signal\n");
    WiFi.scanNetworksAsync([=](int a) { this->scanComplete(a); }, false);
}

void WiFiAutoSwitchClass::scanComplete(int scanResult) {
    if(scanResult == 0) {
        INFO("[MULTIWIFI] no networks available\n");
        return;
    }
    INFO("[MULTIWIFI] %d networks found\n", scanResult);
    WiFiCredential bestNetwork { NULL, NULL };
    int bestNetworkDb = INT_MIN;
    uint8 bestBSSID[6];
    int32_t bestChannel;
    delay(0);

    for (int8_t i = 0; i < scanResult; ++i) {
        String ssid_scan;
        int32_t rssi_scan;
        uint8_t sec_scan;
        uint8_t* BSSID_scan;
        int32_t chan_scan;
        bool hidden_scan;
        uint8_t* BSSID = nullptr;
        
        if (WiFi.status() == WL_CONNECTED) BSSID = WiFi.BSSID();

        WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);

        bool known = false;
        for (auto entry : _wifilist) {
            if (ssid_scan == entry.SSID) { // SSID match
                known = true;
                if (rssi_scan > bestNetworkDb) { // best network
                    if (sec_scan == ENC_TYPE_NONE || entry.pwd) { // check for passphrase if not open wlan
                        if (!BSSID || BSSID[0] != BSSID_scan[0] || BSSID[1] != BSSID_scan[1] || BSSID[2] != BSSID_scan[2] || BSSID[3] != BSSID_scan[3] || BSSID[4] != BSSID_scan[4] || BSSID[5] != BSSID_scan[5]) {
                            bestNetworkDb = rssi_scan;
                            bestChannel = chan_scan;
                            bestNetwork = entry;
                            memcpy((void*) &bestBSSID, (void*) BSSID_scan, sizeof(bestBSSID));
                        }
                    }
                }
                break;
            }
        }

        if(known) {
            INFO(" ---> ");
        } else {
            INFO("      ");
        }

        INFO(" %d: [%d][%02X:%02X:%02X:%02X:%02X:%02X] %s (%d) %c\n", i, chan_scan, BSSID_scan[0], BSSID_scan[1], BSSID_scan[2], BSSID_scan[3], BSSID_scan[4], BSSID_scan[5], ssid_scan.c_str(), rssi_scan, (sec_scan == ENC_TYPE_NONE) ? ' ' : '*');
        delay(0);
    }
    WiFi.scanDelete();
    if (bestNetwork.SSID) {
        INFO("[MULTIWIFI] Connecting BSSID: %02X:%02X:%02X:%02X:%02X:%02X SSID: %s Channel: %d (%d)\n", bestBSSID[0], bestBSSID[1], bestBSSID[2], bestBSSID[3], bestBSSID[4], bestBSSID[5], bestNetwork.SSID, bestChannel, bestNetworkDb);
        WiFi.persistent(false);
        WiFi.begin(bestNetwork.SSID, bestNetwork.pwd, bestChannel, bestBSSID);
        WiFi.setAutoReconnect(true);
    }
}

void WiFiAutoSwitchClass::add(const char* ssid, const char *passphrase) {
    WiFiCredential cred;
    cred.SSID = strdup(ssid);
    if (passphrase) {
        cred.pwd = strdup(passphrase);
    } else {
        cred.pwd = strdup("");
    }
    _wifilist.push_back(cred);
}

void WiFiAutoSwitchClass::findBetterAP() {
    INFO("[MULTIWIFI] looking for better signal\n");
    int8_t scanResult = WiFi.scanNetworks();
    INFO("[MULTIWIFI] scan result: %d\n", scanResult);
    scanComplete(scanResult);
}


WiFiAutoSwitchClass WiFiAutoSwitch;
