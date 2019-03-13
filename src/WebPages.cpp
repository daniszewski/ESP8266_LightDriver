#include "WebPages.h"
#include "WebStats.h"
#include "Commands.h"
#include "detail/mimetable.h"

#define TEMPORARY_FILE "/_uploaded"
#define PREFIX_WWW "/www"
#define MIME_TEXTPLAIN mimeTable[3].mimeType

using namespace mime;

ESP8266WebServer server(80);

String prefix = PREFIX_WWW;

void sendOK() { server.send(200, MIME_TEXTPLAIN, "OK"); }
void sendERR(String msg) { server.send(500, MIME_TEXTPLAIN, msg); }
void sendRedirect(String url) { server.sendHeader("Location", url, true); server.send(302, MIME_TEXTPLAIN, ""); }
void sendNoAdmin() { server.send(500, MIME_TEXTPLAIN, "Not admin"); }

void zeroConf();

void WebPagesClass::begin() {
    server.on("/stats", HTTP_GET, [this](){ handleJson(getStats()); });
    server.on("/boot", [this](){ handleBoot(); });
    server.on("/run", [this](){ handleRun(); });
    //server.on("/dir", HTTP_GET, [this](){ if (isAdmin()) handleDir("/www"); else sendNoAdmin();});
    //server.on("/scripts", HTTP_GET, [this](){ if (isAdmin()) handleDir("/scripts"); else sendNoAdmin();});
    server.on("/crc", HTTP_GET, [this](){ 
        if (isAdmin()) handleJson("{ \"crc\": \""+ESP.getSketchMD5()+"\" }"); 
        else sendNoAdmin(); 
    });
    server.on("/upload", HTTP_POST, [](){ 
        if (isAdmin()) {
            SPIFFS.remove(prefix + "/" + server.arg(0));
            SPIFFS.rename(TEMPORARY_FILE, prefix + "/" + server.arg(0)); 
            server.send(200); 
        } else sendNoAdmin();
    }, [this](){ handleFileUpload(TEMPORARY_FILE); });
    server.on("/upload", HTTP_PUT, [](){ 
        if (isAdmin()) server.send(200); 
        else sendNoAdmin();
    }, [this](){ handleFileUpload(server.upload().filename); });
    server.on("/update", HTTP_POST, []() {
        if (isAdmin()) {
            sendOK();
            delay(500);
            ESP.restart();
        } else sendNoAdmin();
    }, [this]() { handleFirmwareUpdate(); });
    server.onNotFound([this](){ handleStaticPage(); });
    server.begin();
}

void WebPagesClass::handle() {
    server.handleClient();
}

String WebPagesClass::getContentType(const String& path) {
    char buff[sizeof(mimeTable[0].mimeType)];
    // Check all entries but last one for match, return if found
    for (size_t i=0; i < sizeof(mimeTable)/sizeof(mimeTable[0])-1; i++) {
        strcpy_P(buff, mimeTable[i].endsWith);
        if (path.endsWith(buff)) {
            strcpy_P(buff, mimeTable[i].mimeType);
            return String(buff);
        }
    }
    // Fall-through and just return default type
    strcpy_P(buff, mimeTable[sizeof(mimeTable)/sizeof(mimeTable[0])-1].mimeType);
    return String(buff);
}

void WebPagesClass::handleStaticPage() {
    String url = server.uri(); url.toLowerCase();
    if (url == "/" && server.method() == HTTP_GET) url = "/index.html";

    if (server.method() == HTTP_GET) {
        if (isAdmin() && (url.startsWith("/dir/") || url=="/dir")) {
            url = url.substring(4);
            if (url.endsWith("/") && url.length() > 1) url = url.substring(0, url.length() - 1);
            handleDir(url);
        } else {
            if (!isAdmin() || !url.startsWith(getScriptsPath())) url = prefix + url;
            File f = SPIFFS.open(url, "r");
            if (f) {
                String h = getContentType(url);
                server.sendHeader("Content-Type", h);
                server.streamFile(f, h);
                f.close();
            } else if (url == prefix + "/index.html") zeroConf();
            else server.send(404, MIME_TEXTPLAIN, "404: Not found");
        }
    } else if (server.method() == HTTP_PUT) {
        if (isAdmin()) {
            if (!url.startsWith(getScriptsPath())) url = prefix + url;
            File f = SPIFFS.open(url, "w");
            if (f) { 
                f.print(server.arg("plain"));
                f.print("\n");
                f.flush(); f.close();
                server.send(200, MIME_TEXTPLAIN, "OK");
            } else server.send(500, MIME_TEXTPLAIN, "File write error");
        } else sendNoAdmin();
    } else if (server.method() == HTTP_DELETE) {
        if (isAdmin()) {
            if (!url.startsWith(getScriptsPath())) url = prefix + url;
            SPIFFS.remove(url);
            sendOK();
        } else sendNoAdmin();
    }
}

void WebPagesClass::handleJson(const String& json) {
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.send(200, "application/json", json);
}

void WebPagesClass::handleDir(const String& root) {
    Dir dir = SPIFFS.openDir(root);
    String list = "{ ";
    bool first = true;
    while (dir.next()) {
        File f = dir.openFile("r");
        if (first) first = false;
        else list += ", ";
        list += "\""+dir.fileName() + "\": " + String(f.size());
        f.close();
    }
    list+=" }";
    handleJson(list);
}

void WebPagesClass::handleBoot() {
    if (isAdmin()) {
        if (server.method() == HTTP_GET) {
            File f = SPIFFS.open("/boot", "r");
            server.setContentLength(f.size());
            server.streamFile(f, MIME_TEXTPLAIN);
            f.close();
        } else if (server.method() == HTTP_PUT) {
            File f = SPIFFS.open("/boot", "w");
            if (f) { 
                f.print(server.arg("plain"));
                f.print("\n");
                f.flush(); f.close();
                sendOK();
                f = SPIFFS.open("/boot_tries", "w");
                f.print("0");
                f.flush(); f.close();
            } else server.send(500, MIME_TEXTPLAIN, "Could not write file /boot");
        }
    } sendNoAdmin();
}

void WebPagesClass::handleRun() {
    if (server.method() == HTTP_GET) {
        String file = getScriptsPath() + server.arg(0);
        file.toLowerCase();
        if (executeFile(file)) sendOK();
        else sendERR(getLastError());
    } else if (server.method() == HTTP_PUT) {
        String body = server.arg("plain");
        int lineStart = 0, lineEnd = 0, lineCounter = 1;
        do {
            lineEnd = body.indexOf('\n',lineStart);
            if (lineEnd<0) lineEnd = body.length();
            String line = body.substring(lineStart, lineEnd); line.trim();
            if (!execute(line)) {
                ERR("Unknown command in line "+String(lineCounter));
                sendERR(getLastError());
                return;
            }
            lineStart = lineEnd + 1; lineCounter++;
        } while ((unsigned int)lineStart<body.length());
        sendOK();
    }
}

File fsUploadFile;
void WebPagesClass::handleFileUpload(const String& filename) {
    if (!isAdmin()) return;

    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START){
        INFO("Uploading file " + filename);
        fsUploadFile = SPIFFS.open(filename, "w");
    } else if (upload.status == UPLOAD_FILE_WRITE){
        if (fsUploadFile) {
            fsUploadFile.write(upload.buf, upload.currentSize);
        }
    } else if (upload.status == UPLOAD_FILE_END){
        if (fsUploadFile) {
            fsUploadFile.close();
        } else {
            sendERR("couldn't create file");
        }
    }
}

void WebPagesClass::handleFirmwareUpdate() {
    if (!isAdmin()) return;

    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace)) { //start with max available size
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
            Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
    }
    yield();
}

void zeroConf() {
    server.send(200, "text/html", "\
<!DOCTYPE html>\
<html><head><title>ESP8266 Lights</title></head><body>\
<h1>ESP8266 ZERO CONF</h1>\
<div style='text-align:right;width:250px'>\
SSID: <input type='text' id='s' /><br />\
PWD: <input type='password' id='p' /><br />\
<input type='button' value='Connect' onclick='wifi()' />\
<input type='button' value='Reset' onclick='reset()' />\
</div><br /><br />\
Client IP: <strong><span style='color:green' id='IP'></span></strong>\
<script>\
function getStats() {rest('GET','stats',null,function() {if (this.readyState==4) { if(this.status==200) {var j=eval('x='+this.responseText);sv('IP',j.WiFiClient_IP);} setTimeout(getStats, 1000);}});}\
function wifi() { sv('IP','connecting...'); rest('PUT','run','LOGIN esplight\\nWIFI '+gv('s')+' '+gv('p')+'\\n'); }\
function reset() { sv('IP','restarting...'); rest('PUT','run','LOGIN esplight\\nRESTART\\n'); }\
function rest(method,url,body,cb) { var x=new XMLHttpRequest();x.onreadystatechange=cb;x.open(method,url,true);x.send(body);}\
function gv(id) { return document.getElementById(id).value;}\
function sv(id,v) { document.getElementById(id).innerText=v;}\
setTimeout(getStats, 1000);\
</script>\
</body></html>");
}

WebPagesClass WebPages;
