#include "PinDriver.h"
#include "PowerAnimator.h"

struct SwitchDef { char type; bool hwState; bool fnState; unsigned long lastCloseTime; String onLow; String onHigh; };
struct PhasePlanStep { int wait; byte pin; byte state; };

const uint8_t pins[PINCOUNTALL] = { D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
char pinType[32] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
SwitchDef switchDef[32];

unsigned short pulseLength = 2;
int phaseStartTime = 2000;
int phaseEndTime = 9000;
bool boot = true;

int zeroCounter = 0;
unsigned long zeroLastCheck = 0; 
float zeroFreq;

// Phase based drived
PhasePlanStep _planSteps[20];
byte _phasePlanPosition;
byte _phasePlanLength;

void phaseStart();
void startTimer(long microsecs);
void zeroDetected();

bool PinDriverClass::getSwitchValue(uint8_t pin) {
    return switchDef[pin].fnState;
}

uint8_t PinDriverClass::getPin(int ix) {
    return pins[ix];
}

uint8_t PinDriverClass::parsePin(String pin) { // D0 D1 ... D9 V0 V1 ... V9
    char t = pin.charAt(0);
    char c = pin.charAt(1);
    if (c < CHR_0) c = CHR_0;
    if (c > CHR_9) c = CHR_9;
    return pins[c - CHR_0 + (t=='D' ? 0 : 10)];
}

void PinDriverClass::initZero(String pinName) {
    uint8_t pin = parsePin(pinName);
    pinType[pin] = 'X'; // cross zero
    if (pin < FIRTSVIRTUALPIN) attachInterrupt(digitalPinToInterrupt(pin), zeroDetected, RISING);
}

void PinDriverClass::setPhaseStartTime(int time) {
    phaseStartTime = time;
}

void PinDriverClass::setPhaseEndTime(int time) {
    phaseEndTime = time;
}

void PinDriverClass::setPulseLength(int time) {
    pulseLength = time;
}

char PinDriverClass::getPinType(uint8_t pin) {
    return pinType[pin];
}

PowerAnimation* PinDriverClass::getPinAnim(uint8_t pin) {
    return PowerAnimator.ensureAnimation(pin);
}

int sortPlanComp(const void *cmp1, const void *cmp2)
{
    return (*((PhasePlanStep *)cmp1)).wait - (*((PhasePlanStep *)cmp2)).wait;
}

int getPowerNonLinear(int value) {
    value = (value * (value+1025L)) >> 11;
    if (value<0) value = 0;
    if (value>1023) value = 1023;
    return value;
}


void phaseStart() {
    _phasePlanLength = 0;
    for (int i=0;i<PINCOUNT;i++) {
        uint8_t p = pins[i];
        if (pinType[p]=='Z') { //ZERO only
            auto anim = PowerAnimator.ensureAnimation(p); //pinAnim[p]
            anim->animate();
            int value = anim->getValue();
            int wait = phaseEndTime - ((getPowerNonLinear(value) * (phaseEndTime - phaseStartTime)) >> 10);
            _planSteps[_phasePlanLength++] = { wait, p, (byte)(value > 0 ? HIGH : LOW) };
            _planSteps[_phasePlanLength++] = { wait + pulseLength, p, (byte)(value == 1023 ? HIGH : LOW) };
        }
    }
    qsort(_planSteps, _phasePlanLength, sizeof(_planSteps[0]), sortPlanComp);
    for(int i=_phasePlanLength-1;i>0;i--) {
        _planSteps[i].wait -= _planSteps[i-1].wait;
    }

    _phasePlanPosition = 0;
    startTimer(_planSteps[_phasePlanPosition].wait);
}

void execAction() {
    digitalWrite(_planSteps[_phasePlanPosition].pin,_planSteps[_phasePlanPosition].state);
    if(++_phasePlanPosition < _phasePlanLength) startTimer(_planSteps[_phasePlanPosition].wait);
}

void IRAM_ATTR onTimerISR(){ execAction(); }

void startTimer(long microsecs) {
    if (microsecs > 0) {
        timer1_write(5 * microsecs);
    } else if (microsecs == 0) {
        execAction();
    } 
}

void setupTimer() {
    timer1_attachInterrupt(onTimerISR);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
}

unsigned long phaseZeroTime = 0;
void zeroDetected() {
    zeroCounter++;
    if (isBoot()) return;
    if (micros() < phaseZeroTime + 2000) return;
    phaseZeroTime = micros();
    phaseStart();
}

void PinDriverClass::updatePinsPwm() {
    for (int i=0;i<PINCOUNT;i++) {
        uint8_t p = pins[i];
        if (pinType[p]=='P') { //PWM only, no D0 - && pins[p]!=20
            analogWrite(p, getPowerNonLinear(PowerAnimator.ensureAnimation(p)->getValue()));
        } else if (pinType[p]=='O') { //ON/OFF
            digitalWrite(p, PowerAnimator.ensureAnimation(p)->getValue() < 512 ? LOW : HIGH);
        }
    }
}

void PinDriverClass::initPin(String pinName, String modeWord) { // mode: PWM, ZERO, ONOFF
    uint8_t pin = parsePin(pinName);
    char mode = modeWord.charAt(0);
    pinType[pin] = mode;
    String output = "0";
    if (pin < FIRTSVIRTUALPIN) {
        if (modeWord.charAt(1)=='1') {
            digitalWrite(pin, LOW);
            output = "HIGH";
        } else if (modeWord.charAt(1)=='0') {
            digitalWrite(pin, HIGH);
            output = "LOW";
        }
        pinMode(pin, OUTPUT);
    }
    PowerAnimator.configStart(pin, mode=='Z' ? HIGH : LOW);
    PowerAnimator.configAddStep(output, "");
    PowerAnimator.configEnd();
}

void PinDriverClass::initSwitch(String pinName, String switchType, String onHigh, String onLow) {
    uint8_t pin = parsePin(pinName);
    onLow.toLowerCase();
    onHigh.toLowerCase();
    pinType[pin] = 'S';
    char type = switchType.charAt(0); // 1, 2 or T/P/Z
    if (type > CHR_9) type = 'T';
    if (pin < FIRTSVIRTUALPIN) pinMode(pin, pin == 16 ? INPUT : INPUT_PULLUP); // pin 16 doesn't have a pullup option in ESP8266
    switchDef[pin] = { type, false, false, millis(), onLow, onHigh };
}

void PinDriverClass::disablePin(String pinName) {
    uint8_t pin = parsePin(pinName);
    if (pinType[pin] == 'X') detachInterrupt(digitalPinToInterrupt(pin));
    pinType[pin] = ' ';
    if (pin < FIRTSVIRTUALPIN) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }
}

void setSwitchState(uint8_t pin, bool fnState) {
    if (switchDef[pin].fnState != fnState) {
        switchDef[pin].fnState = fnState;
        executeFile(getScriptsPath() + (fnState ? switchDef[pin].onHigh : switchDef[pin].onLow));
    }
}

void onSwitchHandler(uint8_t pin, bool newState) {
    const SwitchDef &s = switchDef[pin]; 
    if (s.type == '1') {
        if (newState) setSwitchState(pin, !switchDef[pin].fnState);
    } else if (s.type == '2') {
        setSwitchState(pin, newState);
    } else { // stair switch
        setSwitchState(pin, !switchDef[pin].fnState);
    }
}

void PinDriverClass::handleSwitches() {
    unsigned long ct = millis();
    if (boot && ct < BOOT_DELAY) return;
    boot = false;

    for (int i=0;i<PINCOUNT;i++) {
        uint8_t p = pins[i];
        if (pinType[p] != 'S') continue; //SWITCH only

        bool closed = digitalRead(p) == LOW; // HIGH - open, LOW - closed
        if (closed) {
            switchDef[p].lastCloseTime = ct;
        } else {
            if (ct < switchDef[p].lastCloseTime) switchDef[p].lastCloseTime = 0; // millis() overflow handling - should be really rare but still
            if (ct - switchDef[p].lastCloseTime < ZERO_DELAY_THRESHOLD) closed = true;
        }
        if (switchDef[p].hwState != closed) {
            switchDef[p].hwState = closed;
            onSwitchHandler(p, closed);
        }
    }
}

void PinDriverClass::turnSwitch(String pinName, String state) {
    uint8_t pin = parsePin(pinName);
    bool targetState = state=="1" ? true : state=="0" ? false : !switchDef[pin].fnState;
    onSwitchHandler(pin, targetState);
}

void PinDriverClass::handleZeroCounter() {
    unsigned long ct = millis();
    if (ct > zeroLastCheck + 2000) {
        zeroFreq = zeroCounter*1000.0/(ct-zeroLastCheck);
        zeroLastCheck = ct;
        zeroCounter = 0;
    } else if (ct < zeroLastCheck) { // millis() overflow
        zeroLastCheck = ct;
        zeroCounter = 0;
    }
}

float PinDriverClass::getZeroFreq() {
    return zeroFreq;
}

int proximitySortFunc(const void *cmp1, const void *cmp2)
{
    return *((unsigned int *)cmp2) - *((unsigned int *)cmp1);
}

unsigned int PinDriverClass::getProximity(String triggerPin, String echoPin, int reads, int avgrange) {
    uint8_t pin1 = parsePin(triggerPin);
    uint8_t pin2 = parsePin(echoPin);
    if (!reads) reads = 15;
    if (!avgrange) avgrange = 5;
    unsigned int *buffer = (unsigned int *) malloc(reads*sizeof(unsigned int));
    pinMode(pin1, OUTPUT);
    pinMode(pin2, INPUT);

    for (int i=0; i<reads; i++) {
        if(i) delay(50);
        digitalWrite(pin1, LOW); delayMicroseconds(2);
        digitalWrite(pin1, HIGH); delayMicroseconds(10);
        digitalWrite(pin1, LOW);
        buffer[i] = pulseIn(pin2, HIGH) * 10 / 58;
    }
    qsort(buffer, reads, sizeof(unsigned int), proximitySortFunc);
    unsigned int sum = 0;
    for (int i=0; i<avgrange; i++) {
        sum += buffer[((reads-avgrange)>>1)+i];
    }
    free(buffer);
    return sum / avgrange;
}

void PinDriverClass::begin() {
    setupTimer();
    PowerAnimator.setCallback([](){ PinDriver.updatePinsPwm(); });
}

void PinDriverClass::handle() {
    handleSwitches();
    handleZeroCounter();
}

PinDriverClass PinDriver; // Singleton
