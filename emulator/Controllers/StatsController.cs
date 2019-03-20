using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using ESP8266DriverEmu.Driver;
using Microsoft.AspNetCore.Mvc;

namespace ESP8266DriverEmu.Controllers
{
    [Route("stats")]
    [ApiController]
    public class StatsController : ControllerBase
    {
        [HttpGet]
        public ActionResult<IDictionary<string, object>> Get()
        {
            var result = new Dictionary<string, object>();
            result.Add("name", DriverEmu.Instance.driverName);
            result.Add("freemem", 32784);
            result.Add("admin", DriverEmu.Instance.isAdmin() ? 1: 0);
            foreach(var kvp in DriverEmu.Instance.PinDriver.Pins) {
                var d = new Dictionary<string,object>();
                var pin = kvp.Value;
                if(pin is PinSwitch) {
                    d.Add("type", "SWITCH");
                    d.Add("value", DriverEmu.Instance.PinDriver.getSwitchValue(kvp.Key));
                } else { //PinLight
                    d.Add("type", "LIGHT");
                    d.Add("value", (pin as PinLight).anim.getValue());
                }
                result.Add(kvp.Key, d);
            }
            result.Add("WiFiClient_SSID", "EMU");
            result.Add("WiFiClient_IP", "127.0.0.1");
            result.Add("WiFiAP_SSID", "EMU_AP");
            result.Add("WiFiAP_IP", "127.0.0.1");
            result.Add("version", DriverEmu.Instance.version);

            return result;
        }
    }
}
