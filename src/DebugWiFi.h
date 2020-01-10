#ifndef NDEBUG

#include <ESP8266WiFi.h>

#define DebugWifi WiFi.onEvent(eventWiFi);

void dbg_printf ( const char *format, ... )
{
    static char sbuf[1400];                                                     // For debug lines
    va_list varArgs;                                                            // For variable number of params
    va_start ( varArgs, format );                                               // Prepare parameters
    vsnprintf ( sbuf, sizeof ( sbuf ), format, varArgs );                       // Format the message
    va_end ( varArgs );                                                         // End of using parameters
    Serial.print ( sbuf );
}

String connectionStatus ( int which )
{
    switch (which)
    {
        case WL_CONNECTED:
            return "Connected";
            break;
        case WL_NO_SSID_AVAIL:
            return "Network not availible";
            break;
        case WL_CONNECT_FAILED:
            return "Wrong password";
            break;
        case WL_IDLE_STATUS:
            return "Idle status";
            break;
        case WL_DISCONNECTED:
            return "Disconnected";
            break;
        default:
            return "Unknown";
            break;
    }
}

void eventWiFi(WiFiEvent_t event) {
    switch(event) {
        case WIFI_EVENT_STAMODE_CONNECTED:
            dbg_printf("[WiFi] %d, Connected\n", event);
            break;
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            dbg_printf("[WiFi] %d, Disconnected - Status %d, %s\n", event, WiFi.status(), connectionStatus( WiFi.status() ).c_str() );      
            break;
        case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:
            dbg_printf("[WiFi] %d, AuthMode Change\n", event);
            break;
        case WIFI_EVENT_STAMODE_GOT_IP:
            dbg_printf("[WiFi] %d, Got IP %s\n", event, WiFi.localIP().toString().c_str());
            break;
        case WIFI_EVENT_STAMODE_DHCP_TIMEOUT:
            dbg_printf("[WiFi] %d, DHCP Timeout\n", event);
            break;
        case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
            dbg_printf("[AP] %d, Client Connected\n", event);
            break;
        case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
            dbg_printf("[AP] %d, Client Disconnected\n", event);
            break;
        case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
            dbg_printf("[AP] %d, Probe Request Recieved\n", event);
            break;
    }
}

#else

#define DebugWifi WiFi.onStationModeGotIP(&onStationConnected);

void onStationConnected(const WiFiEventStationModeGotIP& evt) {
    Serial.println("IP:"+evt.ip.toString());
}

#endif
