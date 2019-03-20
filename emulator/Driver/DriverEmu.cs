using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Threading.Tasks;

namespace ESP8266DriverEmu.Driver
{
    public class DriverEmu : IDisposable
    {
        #region Singleton
        static DriverEmu _instance;

        public static DriverEmu Instance
        {
            get
            {
                if (_instance == null) _instance = new DriverEmu();
                return _instance;
            }
        }

        #endregion

        public string version;
        public String adminpwd;
        public String scriptsPath;
        public string driverName;
        public bool _admin;
        public bool _boot;

        LightAnimatorClass LightAnimator;
        public PinDriverClass PinDriver;

        public DriverEmu()
        {
            Init();
        }

        public void ERR(string msg) 
        {
            Console.WriteLine("Error: " +msg);
        }

        public void Init() {
            version = "0.1";
            adminpwd = "esplight";
            scriptsPath = "/scripts/";
            driverName = "emu";
            _admin = false;
            _boot = true;
            if (PinDriver!=null) PinDriver.Dispose();
            if (LightAnimator!=null) LightAnimator.Dispose();
            PinDriver = new PinDriverClass();
            LightAnimator = new LightAnimatorClass();

            executeFile(@"storage\boot.txt");
            bootComplete();
        }
        public void setAdminPassword(String pwd) {
            adminpwd = pwd;
        }

        public void bootComplete() {
            _boot = false;
        }
        public bool isBoot() {
            return _boot;
        }

        public bool isAdmin() {
            return _admin || _boot;
        }

        public void setAdmin() {
            if(_boot) _admin = true;
        }

        public bool login(String password) {
            if (adminpwd == password) {
                _admin = true;
                return true;
            }
            return false;
        }

        public void logout() {
            _admin = false;
        }

        public void setDriverName(String name) {
            driverName = name;
        }

        public String getDriverName() {
            return driverName;
        }

        public bool executeFile(String filename) {
            try {
                var lines = System.IO.File.ReadAllLines(filename);
                int lineNo = 1;
                foreach(var line in lines) {
                    if (!execute(line)) {
                        ERR(filename+": unknown command in line "+(lineNo).ToString());
                    }
                    lineNo++;
                }
                return true; 
            } catch {
                ERR(filename + ": file doesn't exist.");
                return false;
            }
        }

        public bool execute(String line) {
            line = line.Trim();

            String cmd = getWord(line, 0);
            //INFO(line);
            bool adm = isAdmin();
            if      (adm && cmd == "NAME") setDriverName(getWord(line, 1)); // Syntax: NAME <name of the node>
            else if (adm && cmd == "ZERO") PinDriver.initZero(getWord(line, 1)); // Syntax: ZERO <pin name>
            else if (adm && cmd == "PHASESTART") PinDriver.setPhaseStartTime(int.Parse(getWord(line, 1))); // Syntax: PHASESTART <phase start: 0-10000>
            else if (adm && cmd == "PHASEEND") PinDriver.setPhaseEndTime(int.Parse(getWord(line, 1))); // Syntax: PHASEEND <phase end: 0-10000>
            else if (adm && cmd == "PULSE") PinDriver.setPulseLength(int.Parse(getWord(line, 1))); // Syntax: PULSE <pulse length>
            else if (adm && cmd == "LIGHT") PinDriver.initPin(getWord(line, 1), getWord(line, 2)); // Syntax: LIGHT <pin name> <mode: PWM, ZERO>
            else if (adm && cmd == "SWITCH") PinDriver.initSwitch(getWord(line, 1), getWord(line, 2), getWord(line, 3), getWord(line, 4)); // Syntax: SWITCH <pin name> <switch type> <on close> <on open>
            else if (adm && cmd == "DISABLE") PinDriver.disablePin(getWord(line, 1)); // Syntax: DISABLE <pin name>
            else if (adm && cmd == "PWD") setAdminPassword(getWord(line, 1)); // Syntax: PWD <password>
            else if (adm && cmd == "BOOTADMIN") setAdmin(); // Syntax: BOOTADMIN
            else if (adm && cmd == "WIFI") WiFiSTA(getWord(line, 1), getWord(line, 2), true); // Syntax: WIFI <ssid> <password>
            else if (adm && cmd == "WIFITEST") WiFiSTA(getWord(line, 1), getWord(line, 2), false); // Syntax: WIFITEST <ssid> <password>
            else if (adm && cmd == "WIFIAP") WiFiAP(getWord(line, 1)=="1", true); // Syntax: WIFIAP <0 or 1>
            else if (adm && cmd == "WIFIAPTEST") WiFiAP(getWord(line, 1)=="1", false); // Syntax: WIFIAPTEST <0 or 1>
            else if (adm && cmd == "RESTART") restart(); // Syntax: RESTART
            
            else if (cmd == "LOGIN") login(getWord(line, 1)); // Syntax: LOGIN <password>
            else if (cmd == "LOGOFF") logout(); // Syntax: LOGOFF
            else if (cmd == "URL") new WebClient().DownloadStringAsync(new Uri(getWord(line, 1))); // Syntax: URL <url to call>
            else if (cmd == "TURN") PinDriver.turnSwitch(getWord(line, 1), getWord(line, 2)); // Syntax: TURN <pin name> <new state: 1 0 *>
            else if (cmd == "SCRIPT") executeFile(@"storage\\scripts\\" + getWord(line, 1)); // Syntax: SCRIPT <script file>
            else if (cmd == "SEQ") // Syntax: SEQ <pin name>
            {
                string pin = getWord(line, 1); 
                PinDriver.setPinAnim(pin, LightAnimator.configStart(pin, false));
            }
            else if (cmd == "VALUE") LightAnimator.configAddStep(int.Parse(getWord(line, 1)), 0); // Syntax: VALUE <target value>
            else if (cmd == "DELAY") LightAnimator.configAddStep(-1, int.Parse(getWord(line, 1))); // Syntax: DELAY <time of delay>
            else if (cmd == "SLIDE") LightAnimator.configAddStep(int.Parse(getWord(line, 2)), int.Parse(getWord(line, 1))); // Syntax: SLIDE <time of activity> <target value>
            else if (cmd == "REPEAT") LightAnimator.configAddRepeat(int.Parse(getWord(line, 1)), int.Parse(getWord(line, 2))); // Syntax: REPEAT <no of commands> <no of repeats>
            else if (cmd == "END") LightAnimator.configEnd(); // Syntax: END
            else if (cmd != "" && cmd[0] != '#') return false;
            return true;
        }

        public void WiFiAP(bool v1, bool v2)
        {
        }

        public void WiFiSTA(string v1, string v2, bool v3)
        {
        }

        public void restart()
        {
            Init();
        }

        public string getWord(string line, int index) {
            return line.Split(' ')[index];
        }

        public void Dispose()
        {
            if (PinDriver!=null) PinDriver.Dispose();
            if (LightAnimator!=null) LightAnimator.Dispose();
        }
    }
}
