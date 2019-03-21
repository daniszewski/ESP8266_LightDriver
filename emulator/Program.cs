using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;
using ESP8266DriverEmu.Driver;

namespace ESP8266DriverEmu
{
    public class Program
    {
        DriverEmu emu;
        public static void Main(string[] args)
        {
            using(var emu = DriverEmu.Instance) {
                CreateWebHostBuilder(args).Build().Run();
            }
            DriverEmu.Instance = null;
        }

        public static IWebHostBuilder CreateWebHostBuilder(string[] args) =>
            WebHost.CreateDefaultBuilder(args)
                .UseWebRoot("storage/www") // name it whatever you want    
                .UseStartup<Startup>()
            ;
    }
}
