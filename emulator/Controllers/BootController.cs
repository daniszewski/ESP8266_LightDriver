using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace ESP8266DriverEmu.Controllers
{
    [Route("boot")]
    [ApiController]
    public class BootController : ControllerBase
    {
        // GET api/values
        [HttpGet]
        public ActionResult<string> Get()
        {
            if (!ESP8266DriverEmu.Driver.DriverEmu.Instance.isAdmin()) return "Not admin";
            try {
                return System.IO.File.ReadAllText(@"storage/boot");
            } catch (Exception ex) {
                return ex.Message;
            }
        }

        [HttpPut]
        public ActionResult<string> Put()
        {
            if (!ESP8266DriverEmu.Driver.DriverEmu.Instance.isAdmin()) return "Not admin";

            using (StreamReader reader = new StreamReader(Request.Body, Encoding.UTF8))
            {  
                System.IO.File.WriteAllText(@"storage/boot", reader.ReadToEnd()+"\n");
                return "OK";
            }
        }
    }
}
