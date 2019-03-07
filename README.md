# ESP8266_LightDriver
ESP8266 driver with lights dimming functionality. The user can define different animations played in response to any state change of wall switches.

# Handled platforms
All ESP8266-based with at least 2048kB of flash memory.

# Features
- PWM or AC zero crossing light dimming
- 1-state, 2-states and stair switches, closing circuits or by A/C phase detector
- light animations with sequence definitions based on events from switches
- admin mode for setup, OTA updates, etc...
- web server with access to all functions
- communication between different modules via REST Api
- internal scripting language
- live stats
- safe switch for boot loop errors
- virtual switches managed remotely
