#ifndef WEBCALLSQUEUE_H
#define WEBCALLSQUEUE_H

#include <Stream.h>

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
