using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Caching.Distributed;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace ESP8266DriverEmu
{
    public class Startup
    {
        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        public IConfiguration Configuration { get; }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddMvc().SetCompatibilityVersion(CompatibilityVersion.Version_2_1);
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IHostingEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }

            app.UseMvc();
            app.UseFileServer();
            app.MapWhen(x => x.Request.Method == "PUT" && x.Request.Path != "/boot" && x.Request.Path != "/run", HandlePut);
            app.MapWhen(x => x.Request.Method == "DELETE" && x.Request.Path != "/boot" && x.Request.Path != "/run", HandleDelete);
        }
        private static void HandlePut(IApplicationBuilder app)
        {
            app.Run(async context =>
            {
                if (ESP8266DriverEmu.Driver.DriverEmu.Instance.isAdmin()) {
                    var filename = context.Request.Path.ToString().ToLower().Replace("/", "\\").Trim('\\');
                    if (!filename.StartsWith(@"scripts\")) filename = @"www\" + filename;

                    using (var sr = new StreamReader(context.Request.Body))
                    {
                        File.WriteAllText(Path.Combine("storage", filename), sr.ReadToEnd());
                    }
                    await context.Response.WriteAsync("OK");
                } else {
                    await context.Response.WriteAsync("Not admin");
                }
            });
        }

        private static void HandleDelete(IApplicationBuilder app)
        {
            app.Run(async context =>
            {
                if (ESP8266DriverEmu.Driver.DriverEmu.Instance.isAdmin()) {
                    var filename = context.Request.Path.ToString().ToLower().Replace("/", "\\").Trim('\\');
                    if (!filename.StartsWith(@"scripts\")) filename = @"www\" + filename;
                    File.Delete(Path.Combine("storage", filename));
                    await context.Response.WriteAsync("OK");
                } else {
                    await context.Response.WriteAsync("Not admin");
                }
            });
        }
    }
}
