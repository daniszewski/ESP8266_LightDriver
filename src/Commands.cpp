#include "Global.h"
#include "Commands.h"
#include "PinDriver.h"
#include "PowerAnimator.h"
#include "WebCallsQueue.h"
#include "time.h"

bool execute(String line) {
    INFO("%s\n", line.c_str());
    
    line.trim();
    String cmd = getWord(line, 0);
    bool adm = isAdmin();
    if      (adm && cmd == "NAME") setDriverName(getWord(line, 1)); // Syntax: NAME <name of the node>
    else if (adm && cmd == "ZERO") PinDriver.initZero(getWord(line, 1)); // Syntax: ZERO <pin name>
    else if (adm && cmd == "PHASESTART") PinDriver.setPhaseStartTime(getWord(line, 1).toInt()); // Syntax: PHASESTART <phase start: 0-10000>
    else if (adm && cmd == "PHASEEND") PinDriver.setPhaseEndTime(getWord(line, 1).toInt()); // Syntax: PHASEEND <phase end: 0-10000>
    else if (adm && cmd == "PULSE") PinDriver.setPulseLength(getWord(line, 1).toInt()); // Syntax: PULSE <pulse length>
    else if (adm && cmd == "OUTPUT") PinDriver.initPin(getWord(line, 1), getWord(line, 2)); // Syntax: OUTPUT <pin name> <mode: PWM, ZERO, ONOFF>
    else if (adm && cmd == "SWITCH") PinDriver.initSwitch(getWord(line, 1), getWord(line, 2), getWord(line, 3), getWord(line, 4)); // Syntax: SWITCH <pin name> <switch type> <on close> <on open>
    else if (adm && cmd == "DISABLE") PinDriver.disablePin(getWord(line, 1)); // Syntax: DISABLE <pin name>
    else if (adm && cmd == "PWD") setAdminPassword(getWord(line, 1)); // Syntax: PWD <password>
    else if (adm && cmd == "BOOTADMIN") setAdmin(); // Syntax: BOOTADMIN
    else if (adm && cmd == "WIFI") WiFiSTA(getWord(line, 1), getWord(line, 2), getWord(line, 3), true); // Syntax: WIFI <ssid> <password> <channel>
    else if (adm && cmd == "WIFITEST") WiFiSTA(getWord(line, 1), getWord(line, 2), getWord(line, 3), false); // Syntax: WIFITEST <ssid> <password> <channel>
    else if (adm && cmd == "AUTOWIFI") WiFiAdd(getWord(line, 1), getWord(line, 2)); // Syntax: AUTOWIFI <ssid> <password>
    else if (adm && cmd == "WIFIAP") WiFiAP(getWord(line, 1)=="1", true); // Syntax: WIFIAP <0 or 1>
    else if (adm && cmd == "WIFIAPTEST") WiFiAP(getWord(line, 1)=="1", false); // Syntax: WIFIAPTEST <0 or 1>
    else if (adm && cmd == "DELETE") deleteFile(getWord(line, 1)); // Syntax: DELETE <full_filepath>
    else if (adm && cmd == "MQTTINIT") mqttInit(getWord(line, 1), getWord(line, 2), getWord(line, 3), getWord(line, 4)); // Syntax: MQTTINIT <server> <port> <user> <passwd>
    else if (adm && cmd == "MQTTPREFIX") mqttSetPrefix(getWord(line, 1)); // Syntax: MQTTPREFIX <prefix>
    else if (adm && cmd == "MQTTSUB") mqttSubscribe(getWord(line, 1)); // Syntax: MQTTSUB <topic>
    else if (adm && cmd == "RESTART") ESP.restart(); // Syntax: RESTART
    else if (adm && cmd == "CRASH") crash();
    
    else if (cmd == "LOGIN") login(getWord(line, 1)); // Syntax: LOGIN <password>
    else if (cmd == "LOGOFF") logout(); // Syntax: LOGOFF
    else if (cmd == "URL") WebCallsQueue.add(getWord(line, 1)); // Syntax: URL <url to call>
    else if (cmd == "TURN") PinDriver.turnSwitch(getWord(line, 1), getWord(line, 2)); // Syntax: TURN <pin name> <new state: 1 0 *>
    else if (cmd == "SCRIPT") executeFile(getScriptsPath() + getWord(line, 1)); // Syntax: SCRIPT <script file>
    else if (cmd == "MQTT") mqttMessage(getWord(line, 1), getWord(line, 2)); // Syntax: MQTT [<topic>] <value>
    else if (cmd == "TIME") setTime(getWord(line, 1)); // Syntax: TIME <seconds_since_1970>
    else if (cmd == "SEQ") // Syntax: SEQ <pin name>
    {
        uint8_t pin = PinDriver.parsePin(getWord(line, 1)); 
        char type = PinDriver.getPinType(pin);
        PowerAnimator.configStart(pin, type=='Z' ? HIGH : LOW);
    }
    else if (cmd == "VALUE") PowerAnimator.configAddStep(getWord(line, 1), ""); // Syntax: VALUE <target value: 0-1023, ON, OFF, LOW, HIGH>
    else if (cmd == "DELAY") PowerAnimator.configAddStep("-1", getWord(line, 1)); // Syntax: DELAY <time of delay>
    else if (cmd == "SLIDE") PowerAnimator.configAddStep(getWord(line, 2), getWord(line, 1)); // Syntax: SLIDE <time of activity> <target value: 0-1023, ON, OFF, LOW, HIGH>
    else if (cmd == "REPEAT") PowerAnimator.configAddRepeat(getWord(line, 1).toInt(), getWord(line, 2).toInt()); // Syntax: REPEAT <no of commands> <no of repeats>
    else if (cmd == "END") PowerAnimator.configEnd(); // Syntax: END
    else if (cmd != "" && cmd.charAt(0) != '#') return false;
    return true;
}

bool executeScript(String name) {
    return executeFile(name) == -1;
}
