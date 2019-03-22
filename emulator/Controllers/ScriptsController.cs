using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace ESP8266DriverEmu.Controllers
{
    [Route("scripts/{name}")]
    [ApiController]
    public class ScriptsController : ControllerBase
    {
        [HttpGet]
        public ActionResult<string> Get(string name)
        {
            try {
                return System.IO.File.ReadAllText(System.IO.Path.Combine(@"storage/scripts",name));
            } catch (Exception ex) {
                return ex.Message;
            }
        }
    }
}
