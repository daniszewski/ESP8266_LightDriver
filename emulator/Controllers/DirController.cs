using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace ESP8266DriverEmu.Controllers
{
    [Route("dir")]
    [ApiController]
    public class DirController : ControllerBase
    {
        [HttpGet("")]
        public ActionResult<Dictionary<string, int>> Get()
        {
            return Get("www");
        }

        [HttpGet("{name}")]
        public ActionResult<Dictionary<string, int>> Get(string name)
        {
            name = name.Trim('/').Replace('/', '\\');
            if (name.StartsWith(@"scripts")) name = @"www\" + name;
            return System.IO.Directory.GetFiles("..\\data\\"+name)
                .ToDictionary(x => System.IO.Path.GetFileName(x), x => (int)new System.IO.FileInfo(x).Length);
        }
    }
}
