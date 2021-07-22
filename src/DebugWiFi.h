#define INFO_WIFI(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ##__VA_ARGS__ )

#ifdef PROJECT_WIFI_DEBUG

#include <ESP8266WiFi.h>

#define DebugWifi WiFi.onEvent(eventWiFi);

String connectionStatus ( int which )
{
    switch (which)
    {
        case WL_CONNECTED: return PSTR("Connected");
        case WL_NO_SSID_AVAIL: return PSTR("Network not availible");
        case WL_CONNECT_FAILED: return PSTR("Wrong password");
        case WL_IDLE_STATUS: return PSTR("Idle status");
        case WL_DISCONNECTED: return PSTR("Disconnected");
        default: return PSTR("Unknown");
    }
}

void eventWiFi(WiFiEvent_t event) {
    switch(event) {
        case WIFI_EVENT_STAMODE_CONNECTED: INFO_WIFI("[WiFi] %d, Connected\n", event); break;
        case WIFI_EVENT_STAMODE_DISCONNECTED: INFO_WIFI("[WiFi] %d, Disconnected - Status %d, %s\n", event, WiFi.status(), connectionStatus( WiFi.status() ).c_str() ); break;
        case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE: INFO_WIFI("[WiFi] %d, AuthMode Change\n", event); break;
        case WIFI_EVENT_STAMODE_GOT_IP: INFO_WIFI("[WiFi] %d, Got IP %s\n", event, WiFi.localIP().toString().c_str()); break;
        case WIFI_EVENT_STAMODE_DHCP_TIMEOUT: INFO_WIFI("[WiFi] %d, DHCP Timeout\n", event); break;
        case WIFI_EVENT_SOFTAPMODE_STACONNECTED: INFO_WIFI("[AP] %d, Client Connected\n", event); break;
        case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED: INFO_WIFI("[AP] %d, Client Disconnected\n", event); break;
        case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED: INFO_WIFI("[AP] %d, Probe Request Recieved\n", event); break;
        default: break;
    }
}

#else

#define DebugWifi WiFi.onStationModeGotIP(&onStationConnected);

void onStationConnected(const WiFiEventStationModeGotIP& evt) {
    INFO_WIFI("IP: %s\n", evt.ip.toString().c_str());
}

#endif
