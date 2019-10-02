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

int PowerAnimatorClass::animationCount() {
  return animations.GetSize();
}

int PowerAnimatorClass::getValue(int id) {
  for(unsigned int i=0;i<animations.GetSize();i++) {
    if (animations[i].getId() == id) return animations[i].getValue();
  }
  return 0;
}

PowerAnimation* PowerAnimatorClass::ensureAnimation(int id) {
  for(unsigned int i=0;i<animations.GetSize();i++) {
    if (animations[i].getId() == id) return &animations[i];
  }
  PowerAnimation result;
  result.setId(id);
  animations.Add(result);
  return &animations[animations.GetSize()-1];
}

void PowerAnimatorClass::animate() {
  for(unsigned int i=0;i<animations.GetSize();i++) {
    if (!animations[i]._isTriggeredExternaly) animations[i].animate();
  }
}

PowerAnimation* PowerAnimatorClass::configStart(int id, bool isTriggeredExternaly) {
	currentConfigAnimation = ensureAnimation(id);
  currentConfigAnimation->_isTriggeredExternaly = isTriggeredExternaly;
  currentConfigAnimation->clear();
  return currentConfigAnimation;
}

void PowerAnimatorClass::configAddStep(String targetString, unsigned short delay) {
  short target;
  if (targetString == "ON" || targetString == "HIGH") target = 0;
  else if (targetString == "OFF" || targetString == "LOW") target = 1023;
  else if (targetString == "") target = -1;
  else target = (short)targetString.toInt();
	currentConfigAnimation->addStep(delay, target, 0, 0); // do not repeat that
}

void PowerAnimatorClass::configAddRepeat(short commands, short count) {
  currentConfigAnimation->addStep(0, -1, -commands, count);
}

void PowerAnimatorClass::configEnd() {
  currentConfigAnimation->addStep(6000, -1, 0, -1); // every minute jump to itself
  currentConfigAnimation->enable();
}

PowerAnimatorClass PowerAnimator;
