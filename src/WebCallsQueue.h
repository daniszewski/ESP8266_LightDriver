#ifndef WEBCALLSQUEUE_H
#define WEBCALLSQUEUE_H

#include "Global.h"
#include <ESP8266HTTPClient.h>

class WebCallsQueueClass {
    public:
        void begin();
        void handle();
        void add(String url);

    protected:
};

extern WebCallsQueueClass WebCallsQueue;

#endif
