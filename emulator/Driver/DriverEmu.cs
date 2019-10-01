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
            set { _instance = value; }
        }

        #endregion

        public string version;
        public String adminpwd;
        public String scriptsPath;
        public string driverName;
        public bool _admin;
        public bool _boot;

        public string WiFiSSID = "EMU";
        public string WiFiIP = "127.0.0.1";
        public string WiFiAPSSID = "";
        public string WiFiAPIP = "0.0.0.0";

        public PowerAnimatorClass PowerAnimator;
        public PinDriverClass PinDriver;

        public DriverEmu()
        {
            Init();
        }

        public void ERR(string msg)
        {
            Console.WriteLine("Error: " + msg);
        }

        public void Init()
        {
            version = "0.1";
            adminpwd = "espPower";
            scriptsPath = "/scripts/";
            driverName = "emu";
            _admin = false;
            _boot = true;
            if (PinDriver != null) PinDriver.Dispose();
            if (PowerAnimator != null) PowerAnimator.Dispose();
            PinDriver = new PinDriverClass();
            PowerAnimator = new PowerAnimatorClass();

            executeFile(@"storage\boot");
            bootComplete();
        }
        public void setAdminPassword(String pwd)
        {
            adminpwd = pwd;
        }

        public void bootComplete()
        {
            _boot = false;
        }
        public bool isBoot()
        {
            return _boot;
        }

        public bool isAdmin()
        {
            return _admin || _boot;
        }

        public void setAdmin()
        {
            if (_boot) _admin = true;
        }

        public bool login(String password)
        {
            if (adminpwd == password)
            {
                _admin = true;
                return true;
            }
            return false;
        }

        public void logout()
        {
            _admin = false;
        }

        public void setDriverName(String name)
        {
            driverName = name;
        }

        public String getDriverName()
        {
            return driverName;
        }

        public bool executeFile(String filename)
        {
            try
            {
                var lines = System.IO.File.ReadAllLines(filename);
                int lineNo = 1;
                foreach (var line in lines)
                {
                    if (!execute(line))
                    {
                        ERR(filename + ": unknown command in line " + (lineNo).ToString());
                    }
                    lineNo++;
                }
                return true;
            }
            catch (Exception ex)
            {
                ERR(filename + ": " + ex.Message);
                return false;
            }
        }

        public bool execute(String line)
        {
            line = line.Trim();

            String cmd = getWord(line, 0);
            //INFO(line);
            bool adm = isAdmin();
            if (adm && cmd == "NAME") setDriverName(getWord(line, 1)); // Syntax: NAME <name of the node>
            else if (adm && cmd == "ZERO") PinDriver.initZero(getWord(line, 1)); // Syntax: ZERO <pin name>
            else if (adm && cmd == "PHASESTART") PinDriver.setPhaseStartTime(int.Parse(getWord(line, 1))); // Syntax: PHASESTART <phase start: 0-10000>
            else if (adm && cmd == "PHASEEND") PinDriver.setPhaseEndTime(int.Parse(getWord(line, 1))); // Syntax: PHASEEND <phase end: 0-10000>
            else if (adm && cmd == "PULSE") PinDriver.setPulseLength(int.Parse(getWord(line, 1))); // Syntax: PULSE <pulse length>
            else if (adm && cmd == "OUTPUT") PinDriver.initPin(getWord(line, 1), getWord(line, 2)); // Syntax: OUTPUT <pin name> <mode: PWM, ZERO, ONOFF>
            else if (adm && cmd == "SWITCH") PinDriver.initSwitch(getWord(line, 1), getWord(line, 2), getWord(line, 3), getWord(line, 4)); // Syntax: SWITCH <pin name> <switch type> <on close> <on open>
            else if (adm && cmd == "DISABLE") PinDriver.disablePin(getWord(line, 1)); // Syntax: DISABLE <pin name>
            else if (adm && cmd == "PWD") setAdminPassword(getWord(line, 1)); // Syntax: PWD <password>
            else if (adm && cmd == "BOOTADMIN") setAdmin(); // Syntax: BOOTADMIN
            else if (adm && cmd == "WIFI") WiFiSTA(getWord(line, 1), getWord(line, 2), true); // Syntax: WIFI <ssid> <password>
            else if (adm && cmd == "WIFITEST") WiFiSTA(getWord(line, 1), getWord(line, 2), false); // Syntax: WIFITEST <ssid> <password>
            else if (adm && cmd == "WIFIAP") WiFiAP(getWord(line, 1) == "1", true); // Syntax: WIFIAP <0 or 1>
            else if (adm && cmd == "WIFIAPTEST") WiFiAP(getWord(line, 1) == "1", false); // Syntax: WIFIAPTEST <0 or 1>
            else if (adm && cmd == "RESTART") restart(); // Syntax: RESTART

            else if (cmd == "LOGIN") login(getWord(line, 1)); // Syntax: LOGIN <password>
            else if (cmd == "LOGOFF") logout(); // Syntax: LOGOFF
            else if (cmd == "URL") new WebClient().DownloadStringAsync(new Uri(getWord(line, 1))); // Syntax: URL <url to call>
            else if (cmd == "TURN") PinDriver.turnSwitch(getWord(line, 1), getWord(line, 2)); // Syntax: TURN <pin name> <new state: 1 0 *>
            else if (cmd == "SCRIPT") executeFile(@"storage\\scripts\\" + getWord(line, 1)); // Syntax: SCRIPT <script file>
            else if (cmd == "SEQ") // Syntax: SEQ <pin name>
            {
                string pin = getWord(line, 1);
                PinDriver.setPinAnim(pin, PowerAnimator.configStart(pin, false));
            }
            else if (cmd == "VALUE") PowerAnimator.configAddStep(int.Parse(getWord(line, 1)), 0); // Syntax: VALUE <target value>
            else if (cmd == "DELAY") PowerAnimator.configAddStep(-1, int.Parse(getWord(line, 1))); // Syntax: DELAY <time of delay>
            else if (cmd == "SLIDE") PowerAnimator.configAddStep(int.Parse(getWord(line, 2)), int.Parse(getWord(line, 1))); // Syntax: SLIDE <time of activity> <target value>
            else if (cmd == "REPEAT") PowerAnimator.configAddRepeat(int.Parse(getWord(line, 1)), int.Parse(getWord(line, 2))); // Syntax: REPEAT <no of commands> <no of repeats>
            else if (cmd == "END") PowerAnimator.configEnd(); // Syntax: END
            else if (cmd != "" && cmd[0] != '#') return false;
            return true;
        }

        public void WiFiAP(bool enabled, bool persist)
        {
            WiFiAPIP = enabled ? "127.0.0.1" : "0.0.0.0";
            WiFiAPSSID = enabled ? "EMU_AP" : "";
        }

        public void WiFiSTA(string ssid, string pwd, bool persist)
        {
            WiFiSSID = ssid;
            WiFiIP = ssid.Length==0 ? "0.0.0.0" : "127.0.0.1";
        }

        public void restart()
        {
            Init();
        }

        public string getWord(string line, int index)
        {
            try
            {
                return line.Split(' ')[index];
            }
            catch
            {
                return "";
            }
        }

        public void Dispose()
        {
            if (PinDriver != null) PinDriver.Dispose();
            if (PowerAnimator != null) PowerAnimator.Dispose();
        }
    }
}
