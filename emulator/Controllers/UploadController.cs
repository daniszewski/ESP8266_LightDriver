﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace ESP8266DriverEmu.Controllers
{
    [Route("upload")]
    [ApiController]
    public class UploadController : ControllerBase
    {
        // GET api/values
        //[HttpGet]
        //public ActionResult<IDictionary<string, object>> Get()
        //{
        //    var result = new Dictionary<string, object>();
        //    result.Add("name", "lights");
        //    result.Add("freemem", 32784);
        //    result.Add("admin", 0);
        //    result.Add("WiFiClient_SSID", "MDLIGHTS");
        //    result.Add("WiFiClient_IP", "192.168.0.203");
        //    result.Add("WiFiAP_SSID", "LIGHTS_68:C6:3A:CB:06:15");
        //    result.Add("WiFiAP_IP", "192.168.4.1");
        //    result.Add("version", "0.9.0.3");

        //    return result;
        //}

        //// GET api/values/5
        //[HttpGet("{id}")]
        //public ActionResult<string> Get(int id)
        //{
        //    return "value";
        //}

        //// POST api/values
        //[HttpPost]
        //public void Post([FromBody] string value)
        //{
        //}

        //// PUT api/values/5
        //[HttpPut("{id}")]
        //public void Put(int id, [FromBody] string value)
        //{
        //}

        //// DELETE api/values/5
        //[HttpDelete("{id}")]
        //public void Delete(int id)
        //{
        //}
    }
}