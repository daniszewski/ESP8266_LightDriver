#ifndef WEBCALLSQUEUE_H
#define WEBCALLSQUEUE_H

#include "Global.h"

class WebCallsQueueClass {
    public:
        void begin();
        void handle();
        void add(String url);

    private:
        int queueSize;
};

extern WebCallsQueueClass WebCallsQueue;

#endif
