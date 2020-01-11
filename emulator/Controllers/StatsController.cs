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
                } else { //PinPower
                    var ppow = pin as PinPower;
                    d.Add("type", ppow.type);
                    if (ppow.type=="ONOFF") {
                        d.Add("value", ppow.anim?.getValue() <512 ? "\"ON\"" : "\"OFF\"");
                    } else {
                        d.Add("value", ppow.anim?.getValue());
                    }
                    d.Add("seqstep", 1);
                    d.Add("timeleft", 100000);
                }
                pins.Add(d);
            }
            result.Add("pins", pins);
            result.Add("WiFiClient_SSID", DriverEmu.Instance.WiFiSSID);
            result.Add("WiFiClient_IP", DriverEmu.Instance.WiFiIP);
            result.Add("WiFiClient_STATUS", DriverEmu.Instance.WiFiStatus);
            result.Add("WiFiClient_CHANNEL", DriverEmu.Instance.WiFiChannel);
            result.Add("WiFiClient_RSSI", DriverEmu.Instance.WiFiRSSI);
            result.Add("WiFiAP_SSID", DriverEmu.Instance.WiFiAPSSID);
            result.Add("WiFiAP_IP", DriverEmu.Instance.WiFiAPIP);
            result.Add("version", "UI DEMO");

            return result;
        }
    }
}
