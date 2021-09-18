# ESP8266 Power Driver

## Commands

1. Non-admin commands (always available)

    - LOGIN {password} - admin login
    - LOGOFF - logoff admin
    - URL {url to call} - call web url, {http://{ip}/run?file={script}} to run remote script
    - TURN {pin name} {new state: 1 0 *} - change state of switch registered under specified pin (also virtual pins: V0, V1, ..., V9)
    - SCRIPT {script file} - run script (from local storage)
    - SEQ {pin name} - start an output sequence definition under specified pin
    - HD44780_FONT {address} {location 0-7} {row1} {row2} {row3} {row4} {row5} {row6} {row7} {row8} - define custom characted
    - HD44780_CURSOR {address} {x} {y} - move cursor to provided location
    - HD44780_PRINT {address} {string} - print text in current location
    - HD44780_PRINTBIG {address} {string} - print large text in current location
    - HD44780_PRINTCUSTOM {address} {string} - print custom characters (0-7)
    - HD44780_BACKLIGHT {address} {0 or 1} - enable or disable the backlight
    - HD44780_BIGCHAR {address} {char} {sequence: X(block), _(space), 0-7(custom), /(up), \(down), >(right), <(left)} - define big char sequence
    - HD44780_CLEAR {address} - clear the panel
    - PROXIMITY {trigger_pin} {echo_pin} {num_of_reads} {avg_of_middle} - read the proximity sensor in mm
    - RETURN {value} - exit the script and return {value} (including REST calls)

2. Sequence commands (available after starting a sequence)

    - VALUE {target value} - instantly assign a value to the output (0-1023, ON, OFF, LOW, HIGH)
    - DELAY {time of delay} - wait specified time, format: [[h:]m:]s[.f]
    - SLIDE {time of activity} {target value} - transpose the current output level to the target value (0-1023) within provided time (same as DELAY command)
    - REPEAT {no of commands} {no of repeats} - repeat last x commands provided number of times (-1 - inifite)
    - END - end current sequence definition

3. Admin commands (available after providing proper password to LOGIN command)

    - NAME {name of the node} - Set full name of this module for www usage
    - HOSTNAME {name of the node} - set name for network
    - ZERO {pin name} - initialize zero cross phase detector on specified pin
    - PHASESTART {phase start: 0-10000} - set the time offset of half-phase starting point in microseconds
    - PHASEEND {phase end: 0-10000} - set the time offset of half-phase ending point in microseconds
    - PULSE {pulse length} - zero cross phase triac steering pulse length in microseconds
    - OUTPUT {pin name} {mode: PWM, ZERO, ONOFF} - setup an output of type "mode" on specified pin. In case of ONOFF the default state is HIGH. To control it just replace ONOFF with O0 or O1.
    - SWITCH {pin name} {switch type: 1 2 X} {on close} {on open} - setup an action button (1-single state, 2-states, X-stair switch)
    - DISABLE {pin name} - disable previously setup pin
    - PWD {password} - set admin password at a boot time
    - BOOTADMIN - admin commands are available without login
    - WIFI {ssid} {password} - connect to WIFI
    - AUTOWIFI {ssid} {password} - add network to automatic Wifi switcher
    - WIFIAP {1 or 0} - enable or disable the AccessPoint mode
    - DELETE {filename} - delete a file from internal storage
    - RESTART - reset the module
    - MQTTINIT {server} {port} {user} {pwd} - start the MQTT connection to the broker server (no ssl)
    - MQTTSUB {topic} - subscribe to a topic
    - MQTTPREFIX {prefix} - set the prefix for MQTT commands
    - MQTT {topic} {message} - send the MQTT message
    - TIME {seconds_since_1970} - set the current time
    - HD44780_INIT {address} {width} {height} - set up the LCD output on specified I2C adress


## Remarks

The {pin name} can be D0, D1, ..., D9 and in case of switches additionaly V0, V1, ..., V9 for virtual switches available by WIFI
