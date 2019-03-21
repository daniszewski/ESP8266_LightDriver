using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace ESP8266DriverEmu.Controllers
{
    [Route("update")]
    [ApiController]
    public class UpdateController : ControllerBase
    {
        [HttpPost()]
        public ActionResult<string> Post()
        {
            if (!ESP8266DriverEmu.Driver.DriverEmu.Instance.isAdmin()) return "Not admin";

            Driver.DriverEmu.Instance.restart();
            return "";
        }
    }
}
