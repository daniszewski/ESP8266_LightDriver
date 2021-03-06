#ifndef PINDRIVER_H
#define PINDRIVER_H

#include "Global.h"
#include "PowerAnimation.h"

#ifndef D9
    #define D9 3
#endif

#define BOOT_DELAY 500
#define ZERO_DELAY_THRESHOLD 100
#define PINCOUNTALL 20
#define PINCOUNT 10
#define FIRTSVIRTUALPIN 22

class PinDriverClass {
    public:
        void begin();
        void handle();

        void initZero(String pinName);
        void setPhaseStartTime(int time);
        void setPhaseEndTime(int time);
        void setPulseLength(int time);
        void initPin(String pinName, String modeWord);
        void initSwitch(String pinName, String switchType, String onHigh, String onLow);
        void disablePin(String pinName);
        uint8_t getPin(int ix);
        uint8_t parsePin(String pinName);
        char getPinType(uint8_t pin);
        PowerAnimation* getPinAnim(uint8_t pin);
        bool getSwitchValue(uint8_t pin);
        void turnSwitch(String pinName, String state);
        void updatePinsPwm();
        float getZeroFreq();
        unsigned int getProximity(String trigger, String echo, int reads, int avgrange);
    protected:
        void handleSwitches();
        void handleZeroCounter();
};

extern PinDriverClass PinDriver;

#endif
