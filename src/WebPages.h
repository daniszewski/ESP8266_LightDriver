#ifndef WEBPAGES_H
#define WEBPAGES_H

#include "Stream.h"

class WebPagesClass {
  public:
    void begin();
    void handle();
  
  protected:
    int lastHttpTextOrigLength;
    void handleStaticPage();
    void handleJson(const String& json);
    void handleBoot();
    void handleRun();
    void handleDir(String root);
    void handleFileUpload(const String& filename);
    void handleFirmwareUpdate();
    String getContentType(const String& path);
};

extern WebPagesClass WebPages;

#endif
