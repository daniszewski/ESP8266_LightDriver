#include "WebPages.h"
#include "WebStats.h"
#include "Commands.h"

#define TEMPORARY_FILE "/_uploaded"
#define PREFIX_WWW "/www"

ESP8266WebServer server(80);

String prefix = PREFIX_WWW;

void sendOK() { server.send(200, "text/plain", "OK"); }
void sendERR(String msg) { server.send(500, "text/plain", msg); }
void sendRedirect(String url) { server.sendHeader("Location", url, true); server.send(302, "text/plain", ""); }
void sendNoAdmin() { server.send(500, "text/plain", "Not admin"); }

void WebPagesClass::begin() {
  server.on("/stats", [this](){ handleJson(getStats()); });
  server.on("/boot", [this](){ handleBoot(); });
  server.on("/run", [this](){ handleRun(); });
  server.on("/dir", [this](){ handleDir(); });
  server.onNotFound([this](){ handleStaticPage(); });
  server.on("/upload", HTTP_POST, [](){ 
    if (isAdmin()) {
      SPIFFS.rename(TEMPORARY_FILE, prefix + "/" + server.arg(0)); 
      server.send(200); 
    } else sendNoAdmin();
  }, [this](){ handleFileUpload(); });
  server.on("/update", HTTP_POST, []() {
    if (isAdmin()) {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    } else sendNoAdmin();
  }, [this]() { handleFirmwareUpdate(); });
  server.begin();
}

void WebPagesClass::handle() {
  server.handleClient();
}

String WebPagesClass::getContentType(String filename) {
  if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/html";
}

void WebPagesClass::handleStaticPage() {
  String url = server.uri(); url.toLowerCase();
  if (url == "/") url = "/index.html";

  if (server.method() == HTTP_GET) {
    File f = SPIFFS.open(prefix+url, "r");
    if (f) {
      String h = getContentType(url);
      server.sendHeader("Content-Type", h);
      server.streamFile(f, h);
      f.close();
    } else server.send(404, "text/plain", "404: Not found");
  } else if (server.method() == HTTP_PUT) {
    if (isAdmin()) {
      File f = SPIFFS.open(prefix+url, "w");
      if (f) { 
        f.print(server.arg("plain"));
        f.print("\n");
        f.flush(); f.close();
        server.send(200, "text/plain", "OK");
      } else server.send(500, "text/plain", "File write error");
    } else sendNoAdmin();
  } else if (server.method() == HTTP_DELETE) {
    if (isAdmin()) {
      SPIFFS.remove(prefix+url);
      sendOK();
    } else sendNoAdmin();
  }
}

void WebPagesClass::handleJson(String json) {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "application/json", json);
}

void WebPagesClass::handleDir() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  Dir dir = SPIFFS.openDir("/www");
  String list = String();
  while (dir.next()) {
    File f = dir.openFile("r");
    list += dir.fileName() + " " + String(f.size()) + "\n";
    f.close();
  }
  server.send(200, "application/json", list);
}

void WebPagesClass::handleBoot() {
  if (isAdmin()) {
    if (server.method() == HTTP_GET) {
      File f = SPIFFS.open("/boot", "r");
      server.setContentLength(f.size());
      server.streamFile(f, "text/plain");
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
      } else server.send(500, "text/plain", "Could not write file /boot");
    } else if (server.method() == HTTP_DELETE) {
      SPIFFS.remove("/boot");
    }
  } sendNoAdmin();
}

void WebPagesClass::handleRun() {
  if (server.method() == HTTP_GET) {
    String file = prefix+"/"+server.arg("file");
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
void WebPagesClass::handleFileUpload() {
  if (!isAdmin()) return;

  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START){
    fsUploadFile = SPIFFS.open(TEMPORARY_FILE, "w");
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

WebPagesClass WebPages;
