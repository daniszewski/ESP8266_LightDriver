# Commands

1. Non-admin commands (always available)
- LOGIN {password} - admin login
- LOGOFF - logoff admin
- URL {url to call} - call web url, http://{ip}/run?file={script} to run remote script
- TURN {pin name} {new state: 1 0 *} - change state of switch registered under specified pin (also virtual pins: V0, V1, ..., V9)
- SCRIPT {script file} - run script (from local storage)
- SEQ {pin name} - start a light sequence definition under specified pin

2. Sequence commands (available after starting a sequence)
- VALUE {target value} - instantly assign a value to the light
- DELAY {time of delay} - wait specified number of 1/100 of a second
- SLIDE {time of activity} {target value} - transpose the current light level to the target value within provided time
- REPEAT {no of commands} {no of repeats} - repeat last x commands provided number of times (-1 - inifite)
- END - end current sequence definition

3. Admin commands (available after providing proper password to LOGIN command)
- NAME {name of the node} - Sets name of this module
- ZERO {pin name} - Initialize zero cross phase detector on specified pin
- PHASESTART {phase start: 0-10000} - sets the time offset of half-phase starting point in microseconds
- PHASEEND {phase end: 0-10000} - sets the time offset of half-phase ending point in microseconds
- PULSE {pulse length} - zero cross phase triac steering pulse length in microseconds
- LED {pin name} {mode: PWM, ZERO} - setup a light of type "mode" on specified pin
- SWITCH {pin name} {switch type: 1 2 X} {on close} {on open} - setup an action button (1-single state, 2-states, X-stair switch)
- DISABLE {pin name} - disable previously setup pin
- PWD {password} - set admin password at a boot time
- BOOTADMIN - no need for admin to login
- WIFI {ssid} {password} - connect to WIFI (not needed on boot time)

# Remarks

The {pin name} can be D0, D1, ..., D9 and in case of switches additionaly V0, V1, ..., V9 for virtual switches available by WIFI
