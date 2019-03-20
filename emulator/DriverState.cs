using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace ESP8266DriverEmu
{
    public class DriverState
    {
        static DriverState _instance;

        public static DriverState Instance
        {
            get
            {
                if (_instance == null) _instance = new DriverState();
                return _instance;
            }
        }

        public string Name { get; set; }
    }
}
