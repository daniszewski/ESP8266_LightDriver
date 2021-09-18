#include "Global.h"
#include "Commands.h"
#include "PinDriver.h"
#include "PowerAnimator.h"
#include "WebCallsQueue.h"
#include "time.h"
#include "I2C.h"

#define WI(X) getWord(line, X).toInt()
#define WS(X) getWord(line, X)
#define WM(X) getWord(line, X, true)

String execute(String line) {
    INFO("%s\n", line.c_str());
    
    line.trim();
    String cmd = WS(0);
    bool adm = isAdmin();
    if      (adm && cmd == "NAME") setDriverName(WM(1)); // Syntax: NAME <name of the node>
    else if (adm && cmd == "HOSTNAME") WiFi.hostname(WM(1)); // Syntax: HOSTNAME <hostname>
    else if (adm && cmd == "ZERO") PinDriver.initZero(WS(1)); // Syntax: ZERO <pin name>
    else if (adm && cmd == "PHASESTART") PinDriver.setPhaseStartTime(WI(1)); // Syntax: PHASESTART <phase start: 0-10000>
    else if (adm && cmd == "PHASEEND") PinDriver.setPhaseEndTime(WI(1)); // Syntax: PHASEEND <phase end: 0-10000>
    else if (adm && cmd == "PULSE") PinDriver.setPulseLength(WI(1)); // Syntax: PULSE <pulse length>
    else if (adm && cmd == "OUTPUT") PinDriver.initPin(WS(1), WS(2)); // Syntax: OUTPUT <pin name> <mode: PWM, ZERO, ONOFF>
    else if (adm && cmd == "SWITCH") PinDriver.initSwitch(WS(1), WS(2), WS(3), WS(4)); // Syntax: SWITCH <pin name> <switch type> <on close> <on open>
    else if (adm && cmd == "DISABLE") PinDriver.disablePin(WS(1)); // Syntax: DISABLE <pin name>
    else if (adm && cmd == "PWD") setAdminPassword(WS(1)); // Syntax: PWD <password>
    else if (adm && cmd == "BOOTADMIN") setAdmin(); // Syntax: BOOTADMIN
    else if (adm && cmd == "WIFI") WiFiSTA(WS(1), WS(2), WS(3)); // Syntax: WIFI <ssid> <password> <channel>
    else if (adm && cmd == "AUTOWIFI") WiFiAdd(WS(1), WS(2)); // Syntax: AUTOWIFI <ssid> <password>
    else if (adm && cmd == "WIFIAP") WiFiAP(WS(1)=="1"); // Syntax: WIFIAP <0 or 1>
    else if (adm && cmd == "DELETE") deleteFile(WM(1)); // Syntax: DELETE <full_filepath>
    else if (adm && cmd == "MQTTINIT") mqttInit(WS(1), WS(2), WS(3), WS(4)); // Syntax: MQTTINIT <server> <port> <user> <passwd>
    else if (adm && cmd == "MQTTPREFIX") mqttSetPrefix(WS(1)); // Syntax: MQTTPREFIX <prefix>
    else if (adm && cmd == "MQTTSUB") mqttSubscribe(WM(1)); // Syntax: MQTTSUB <topic>
    else if (adm && cmd == "RESTART") ESP.restart(); // Syntax: RESTART
    else if (adm && cmd == "CRASH") crash();
    else if (adm && cmd == "HD44780_INIT") I2C.HD44780_add(WI(1), WI(2), WI(3)); // Syntax: HD44780_INIT <address> <width> <height>
    
    else if (cmd == "HD44780_FONT") I2C.HD44780_font(WI(1), WI(2), WI(3), WI(4), WI(5), WI(6), WI(7), WI(8), WI(9), WI(10)); // Syntax: HD44780_FONT <address> <location> <row1> <row2> <row3> <row4> <row5> <row6> <row7> <row8>
    else if (cmd == "HD44780_CURSOR") I2C.HD44780_cursor(WI(1), WI(2), WI(3)); // Syntax: HD44780_CURSOR <address> <x> <y>
    else if (cmd == "HD44780_PRINT") I2C.HD44780_print(WI(1), WM(2)); // Syntax: HD44780_PRINT <address> <string>
    else if (cmd == "HD44780_PRINTBIG") I2C.HD44780_printbig(WI(1), WM(2)); // Syntax: HD44780_PRINTBIG <address> <string>
    else if (cmd == "HD44780_PRINTFONT") I2C.HD44780_printCustom(WI(1), WM(2)); // Syntax: HD44780_PRINTCUSTOM <address> <string>
    else if (cmd == "HD44780_BACKLIGHT") I2C.HD44780_backlight(WI(1), WI(2)); // Syntax: HD44780_BACKLIGHT <address> <0 or 1>
    else if (cmd == "HD44780_BIGCHAR") I2C.HD44780_bigchar(WI(1), WI(2), WM(3)); // Syntax: HD44780_BIGCHAR <address> <char> <sequence: X(block), _(space), 0-7(custom), /(up), \(down)>
    else if (cmd == "HD44780_CLEAR") I2C.HD44780_clear(WI(1)); // Syntax: HD44780_CLEAR <address>
    else if (cmd == "LOGIN") login(WS(1)); // Syntax: LOGIN <password>
    else if (cmd == "LOGOFF") logout(); // Syntax: LOGOFF
    else if (cmd == "URL") WebCallsQueue.add(WS(1)); // Syntax: URL <url to call>
    else if (cmd == "TURN") PinDriver.turnSwitch(WS(1), WS(2)); // Syntax: TURN <pin name> <new state: 1 0 *>
    else if (cmd == "SCRIPT") executeFile(getScriptsPath() + WS(1)); // Syntax: SCRIPT <script file>
    else if (cmd == "MQTT") mqttMessage(WS(1), WS(2)); // Syntax: MQTT [<topic>] <value>
    else if (cmd == "TIME") setTime(WS(1)); // Syntax: TIME <seconds_since_1970>
    else if (cmd == "RETURN") return WM(1); // Syntax: RETURN <value>
    else if (cmd == "BOARD") return ARDUINO_BOARD; // Syntax: BOARD
    else if (cmd == "PROXIMITY") return String(PinDriver.getProximity(WS(1), WS(2), WI(3), WI(4))); // Syntax: PROXIMITY <trigger_pin> <echo_pin> <num_of_reads> <avg_of_middle>

    else if (cmd == "SEQ") // Syntax: SEQ <pin name>
    {
        uint8_t pin = PinDriver.parsePin(WS(1)); 
        char type = PinDriver.getPinType(pin);
        PowerAnimator.configStart(pin, type=='Z' ? HIGH : LOW);
    }
    else if (cmd == "VALUE") PowerAnimator.configAddStep(WS(1), ""); // Syntax: VALUE <target value: 0-1023, ON, OFF, LOW, HIGH>
    else if (cmd == "DELAY") PowerAnimator.configAddStep("-1", WS(1)); // Syntax: DELAY <time of delay>
    else if (cmd == "SLIDE") PowerAnimator.configAddStep(WS(2), WS(1)); // Syntax: SLIDE <time of activity> <target value: 0-1023, ON, OFF, LOW, HIGH>
    else if (cmd == "REPEAT") PowerAnimator.configAddRepeat(WI(1), WI(2)); // Syntax: REPEAT <no of commands> <no of repeats>
    else if (cmd == "END") PowerAnimator.configEnd(); // Syntax: END
    else if (cmd.length() > 0 && cmd.charAt(0) != '#') return PSTR("Unknown command: ") + cmd;
    return "";
}

String executeScript(String name) {
    return executeFile(name);
}
