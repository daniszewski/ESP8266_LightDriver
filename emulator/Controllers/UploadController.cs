using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace ESP8266DriverEmu.Controllers
{
    [Route("upload")]
    [ApiController]
    public class UploadController : ControllerBase
    {
        [HttpPost()]
        public ActionResult<string> Post()
        {
            if (!ESP8266DriverEmu.Driver.DriverEmu.Instance.isAdmin()) return "Not admin";

            if (Request.Form.Files.Count == 0) return "No file";
            var filename = Request.Form["filename"][0].ToLower().Replace("/", "\\").Trim('\\');
            if (!filename.StartsWith(@"scripts\")) filename = @"www\" + filename;

            using (var br = new BinaryReader(Request.Form.Files[0].OpenReadStream()))
            {
                System.IO.File.WriteAllBytes(@"storage\" + filename, br.ReadBytes((int)Request.Form.Files[0].Length));
            }
            return "OK";
        }
    }
}
