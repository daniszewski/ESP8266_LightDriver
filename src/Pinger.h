#ifndef Pinger_H
#define Pinger_H

#include <stdint.h>
#include <Stream.h>

class PingerClass {
public:
    void begin();
    void handle();
    void start(String ip, int bootDelay, int checkingTime, String scriptName);
private:
};


extern PingerClass Pinger;

#endif
