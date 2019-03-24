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
            List<Dictionary<string, object>> pins = new List<Dictionary<string, object>>();
            foreach (var kvp in DriverEmu.Instance.PinDriver.Pins) {
                var d = new Dictionary<string,object>();
                var pin = kvp.Value;
                d.Add("name", kvp.Key);
                if (pin is PinZero) {
                    d.Add("type", "ZERO");
                    d.Add("value", "100");
                } else if (pin is PinSwitch) {
                    d.Add("type", "SWITCH");
                    d.Add("value", DriverEmu.Instance.PinDriver.getSwitchValue(kvp.Key));
                } else { //PinLight
                    d.Add("type", "LIGHT");
                    d.Add("value", (pin as PinLight).anim?.getValue());
                }
                pins.Add(d);
            }
            result.Add("pins", pins);
            result.Add("WiFiClient_SSID", DriverEmu.Instance.WiFiSSID);
            result.Add("WiFiClient_IP", DriverEmu.Instance.WiFiIP);
            result.Add("WiFiAP_SSID", DriverEmu.Instance.WiFiAPSSID);
            result.Add("WiFiAP_IP", DriverEmu.Instance.WiFiAPIP);
            result.Add("version", DriverEmu.Instance.version);

            return result;
        }
    }
}
