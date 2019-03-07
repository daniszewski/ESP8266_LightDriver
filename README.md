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

# Build instruction
There are two ways to build it and deploy to the hardware:

1. Arduino platform
    Requirements
    - ESP8266 boards (version 2.4.2) from http://arduino.esp8266.com/stable/package_esp8266com_index.json
    To build use src.ino file to open the sketch and build it
2. VS Code
    Requirements
    - PlatformIO extension
    To build use Build task

To maintain compatibility with Arduino environment all the source code files must be placed in one folder (src) along with empty src.ino file.