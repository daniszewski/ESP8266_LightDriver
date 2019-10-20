using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace ESP8266DriverEmu.Controllers
{
    [Route("run")]
    [ApiController]
    public class RunController : ControllerBase
    {
        [HttpGet("{file}")]
        public ActionResult<string> Get(string file)
        {
            Run(System.IO.File.ReadAllText(@"..\data\www\scripts\" + file));
            return "OK";
        }

        [HttpPut()]
        public ActionResult<string> Put()
        {
            using (StreamReader reader = new StreamReader(Request.Body, Encoding.UTF8))
            {
                Run(reader.ReadToEnd());
                return "OK";
            }
        }

        void Run(string content)
        {
            var i = ESP8266DriverEmu.Driver.DriverEmu.Instance;
            int lineNo = 1;
            foreach (var line in content.Split('\n'))
            {
                if (!i.execute(line))
                {
                    i.ERR("Unknown command in line " + (lineNo).ToString());
                }
                lineNo++;
            }
        }
    }
}
