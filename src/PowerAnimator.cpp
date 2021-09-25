#include "PowerAnimator.h"

PowerAnimation *currentConfigAnimation;

std::function<void(void)> _callback;

void PowerAnimatorClass::begin(std::function<void(void)> callback) {
  lastTime = millis();
  _callback = callback;
}

void PowerAnimatorClass::handle() {
  unsigned long currTime = millis();
  if (currTime - lastTime >= 10) {
    animate();
    if (currTime - lastTime >= 20) lastTime = currTime; // overflow or animation disabled for longer period
    else lastTime += 10; // to make sure that average number of calculations per second is 100
    if(_callback) _callback();
  }
}

int PowerAnimatorClass::getValue(int id) {
  for(auto &a: animations) {
    if (a.getId() == id) return a.getValue();
  }
  return 0;
}

PowerAnimation* PowerAnimatorClass::ensureAnimation(int id) {
  for(auto &a: animations) {
    if (a.getId() == id) return &a;
  }
  animations.push_back(PowerAnimation());
  PowerAnimation * result = &animations.back();
  result->setId(id);
  return result;
}

void PowerAnimatorClass::animate() {
  for(auto &a: animations) {
    if (!a._isTriggeredExternaly) a.animate();
  }
}

PowerAnimation* PowerAnimatorClass::configStart(int id, bool isTriggeredExternaly) {
	currentConfigAnimation = ensureAnimation(id);
  currentConfigAnimation->_isTriggeredExternaly = isTriggeredExternaly;
  currentConfigAnimation->clear();
  return currentConfigAnimation;
}

void PowerAnimatorClass::configAddStep(String targetString, String timeInfo) {
  short target;
  if (targetString == "ON" || targetString == "HIGH") target = 0;
  else if (targetString == "OFF" || targetString == "LOW") target = 1023;
  else if (targetString == "") target = -1;
  else target = (short)targetString.toInt();
	currentConfigAnimation->addStep(parseTime(timeInfo), target, 0, 0); // do not repeat that
}

void PowerAnimatorClass::configAddRepeat(short commands, short count) {
  currentConfigAnimation->addStep(0, -1, -commands, count);
}

void PowerAnimatorClass::configEnd() {
  currentConfigAnimation->addStep(UINT32_MAX, -1, 0, -1); // every minute jump to itself
  currentConfigAnimation->enable();
}

unsigned int PowerAnimatorClass::parseTime(const String &time) {
    unsigned int result = 0;
    unsigned short part = 0;
    unsigned short len = time.length();
    bool dot = false;
    for (unsigned short i=0; i < len; i++)
    {
        char c = time[i];
        if (c >= '0' && c <= '9') part = (unsigned short)(part * 10 + (c - '0'));
        else if (c == ':')
        {
            result = (result + part) * 60;
            part = 0;
        }
        else if (c == '.')
        {
            result = result + part;
            dot = true;
            part = 0;
            if (len - i == 2) part = (unsigned short)((time[++i] - '0') * 10);
            else if (len - i == 3) part = (unsigned short)((time[i + 1] - '0') * 10 + (time[i + 2] - '0'));
            break;
        }
    }
    if (!dot) result = (result + part) * 100;
    else result = result * 100 + part;
    return result;
}

PowerAnimatorClass PowerAnimator;
