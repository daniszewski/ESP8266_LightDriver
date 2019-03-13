# First WiFi setup without web interface

1. Attach ESP8266 module to USB
2. Burn firmware to Flash memory
3. Connect to WiFi provided by module: SSID: LIGHT_<MAC> password: esplight
    - If you have problems with receiving the IP from internal DHCP server please use static: 192.168.4.2
4. Browse a web page hosted on url http://192.168.4.1/
5. Put your WIFI credentials on the form and click "Connect"
6. If WIFI creds are ok you should get assigned local IP
7. Use provided script "updates\update.ps1" to update all modules connected to local network. Before runing it check the first lines and adjust the configuration.
