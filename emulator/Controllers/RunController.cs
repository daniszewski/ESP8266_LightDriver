using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace ESP8266DriverEmu.Controllers
{
    [Route("run")]
    [ApiController]
    public class RunController : ControllerBase
    {
        [HttpGet("{name}")]
        public ActionResult<string> Get(string name)
        {
            return Put(System.IO.File.ReadAllText(@"storage\scripts\" + name));
        }

        [HttpPut()]
        public ActionResult<string> Put([FromBody] string content)
        {

            return "OK";
        }
    }
}
