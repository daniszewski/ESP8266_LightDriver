#include "Pinger.h"
#include <vector>

class PingerHostClass {
public:
    String IP;
    int BootDelay;
    int CheckingTime;
    String CallbackScript;
private:
};

std::vector<PingerHostClass> pingerList;

void PingerClass::begin() {
}

void PingerClass::handle() {
}

void PingerClass::start(String ip, int bootDelay, int checkingTime, String scriptName) {
}

PingerClass Pinger;
