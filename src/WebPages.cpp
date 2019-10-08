#include "WebPages.h"
#include "WebStats.h"
#include "Commands.h"
#include "detail/mimetable.h"

#define MIME_TEXTPLAIN mimeTable[3].mimeType

using namespace mime;

ESP8266WebServer server(80);

static const char * STR_OK = "OK";
static const char * STR_PLAIN = "plain";
static const char * STR_R = "r";
static const char * STR_W = "w";
static const char * STR_INDEXHTML = "/index.html";
static const char * STR_WWW = "/www";
static const char * STR_TEMPORARY_FILE = "/_uploaded";

String prefix = STR_WWW;

void sendOK() { server.send(200, MIME_TEXTPLAIN, STR_OK); }
void sendERR(String msg) { server.send(500, MIME_TEXTPLAIN, msg); }
void sendRedirect(String url) { server.sendHeader(F("Location"), url, true); server.send(302, MIME_TEXTPLAIN, ""); }
void sendNoAdmin() { server.send(500, MIME_TEXTPLAIN, F("Not admin")); }

void zeroConf();

void WebPagesClass::begin() {
    server.on(F("/stats"), HTTP_GET, [this](){ sendStats(&server); });
    server.on(F("/boot"), [this](){ handleBoot(); });
    server.on(F("/run"), [this](){ handleRun(); });
    server.on(F("/crc"), HTTP_GET, [this](){ 
        if (isAdmin()) handleJson("{ \"crc\": \""+ESP.getSketchMD5()+"\" }"); 
        else sendNoAdmin(); 
    });
    server.on(F("/upload"), HTTP_POST, [](){ 
        if (isAdmin()) {
            SPIFFS.remove(prefix + "/" + server.arg(0));
            SPIFFS.rename(STR_TEMPORARY_FILE, prefix + "/" + server.arg(0)); 
            server.send(200); 
        } else sendNoAdmin();
    }, [this](){ handleFileUpload(STR_TEMPORARY_FILE); });
    server.on(F("/upload"), HTTP_PUT, [](){ 
        if (isAdmin()) server.send(200); 
        else sendNoAdmin();
    }, [this](){ handleFileUpload(server.upload().filename); });
    server.on(F("/update"), HTTP_POST, []() {
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
    if (url == "/" && server.method() == HTTP_GET) url = STR_INDEXHTML;

    if (server.method() == HTTP_GET) {
        if (url.startsWith(F("/dir/")) || url==F("/dir")) {
            url = url.substring(4);
            if (url.endsWith("/") && url.length() > 1) url = url.substring(0, url.length() - 1);
            handleDir(url);
        } else {
            if (!isAdmin() || !url.startsWith(getScriptsPath())) url = prefix + url;
            File f = SPIFFS.open(url, STR_R);
            if (f) {
                String h = getContentType(url);
                server.sendHeader(F("Content-Type"), h);
                server.streamFile(f, h);
                f.close();
            } else if (url == prefix + STR_INDEXHTML) zeroConf();
            else server.send(404, MIME_TEXTPLAIN, F("404: Not found"));
        }
    } else if (server.method() == HTTP_PUT) {
        if (isAdmin()) {
            if (!url.startsWith(getScriptsPath())) url = prefix + url;
            File f = SPIFFS.open(url, STR_W);
            if (f) { 
                f.print(server.arg(STR_PLAIN));
                f.print("\n");
                f.flush(); f.close();
                server.send(200, MIME_TEXTPLAIN, STR_OK);
            } else server.send(500, MIME_TEXTPLAIN, F("File write error"));
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
    server.sendHeader(F("Cache-Control"), F("no-cache, no-store, must-revalidate"));
    server.sendHeader(F("Pragma"), F("no-cache"));
    server.sendHeader(F("Expires"), F("-1"));
    server.send(200, F("application/json"), json);
}

void WebPagesClass::handleDir(String root) {
    if (root=="") root = STR_WWW;
    Dir dir = SPIFFS.openDir(root);
    String list = "{ ";
    bool first = true;
    while (dir.next()) {
        File f = dir.openFile(STR_R);
        if (first) first = false;
        else list += F(", ");
        list += "\""+dir.fileName().substring(root.length() + 1) + "\": " + String(f.size());
        f.close();
    }
    list+=F(" }");
    handleJson(list);
}

void WebPagesClass::handleBoot() {
    if (isAdmin()) {
        if (server.method() == HTTP_GET) {
            File f = SPIFFS.open(F("/boot"), STR_R);
            if (f) {
                server.setContentLength(f.size());
                String h = getContentType(F("/boot"));
                server.sendHeader(F("Content-Type"), h);
                server.streamFile(f, h);
                f.close();
            } else server.send(500, MIME_TEXTPLAIN, F("Could not read file /boot"));
        } else if (server.method() == HTTP_PUT) {
            File f = SPIFFS.open(F("/boot"), STR_W);
            if (f) { 
                f.print(server.arg(STR_PLAIN));
                f.print("\n");
                f.flush(); f.close();
                sendOK();
                f = SPIFFS.open(F("/boot_tries"), STR_W);
                f.print(F("0"));
                f.flush(); f.close();
            } else server.send(500, MIME_TEXTPLAIN, F("Could not write file /boot"));
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
        String body = server.arg(STR_PLAIN)+'\n';
        int lineStart = 0, lineEnd = 0, lineCounter = 1;
        do {
            lineEnd = body.indexOf('\n',lineStart);
            if (lineEnd<0) lineEnd = body.length();
            String line = body.substring(lineStart, lineEnd); line.trim();
            if (!execute(line)) {
                ERR(PSTR("Unknown command in line ") + String(lineCounter));
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
        INFO(PSTR("Uploading file ") + filename);
        fsUploadFile = SPIFFS.open(filename, STR_W);
    } else if (upload.status == UPLOAD_FILE_WRITE){
        if (fsUploadFile) {
            fsUploadFile.write(upload.buf, upload.currentSize);
        }
    } else if (upload.status == UPLOAD_FILE_END){
        if (fsUploadFile) {
            fsUploadFile.close();
        } else {
            sendERR(F("couldn't create file"));
        }
    }
}

void WebPagesClass::handleFirmwareUpdate() {
    if (!isAdmin()) return;

    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf(PSTR("Update: %s\n"), upload.filename.c_str());
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
            Serial.printf(PSTR("Update Success: %u\nRebooting...\n"), upload.totalSize);
        } else {
            Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
    }
    yield();
}

void zeroConf() {
    server.send(200, F("text/html"), F("\
<!DOCTYPE html>\
<html><head><title>ESP8266 driver zero conf</title></head><body>\
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
function wifi() { sv('IP','connecting...'); rest('PUT','run','LOGIN esppower\\nWIFI '+gv('s')+' '+gv('p')+'\\n'); }\
function reset() { sv('IP','restarting...'); rest('PUT','run','LOGIN esppower\\nRESTART\\n'); }\
function rest(method,url,body,cb) { var x=new XMLHttpRequest();x.onreadystatechange=cb;x.open(method,url,true);x.send(body);}\
function gv(id) { return document.getElementById(id).value;}\
function sv(id,v) { document.getElementById(id).innerText=v;}\
setTimeout(getStats, 1000);\
</script>\
</body></html>"));
}

WebPagesClass WebPages;
