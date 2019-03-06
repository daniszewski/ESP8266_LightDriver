#include "LedAnimator.h"

LedAnimation *currentConfigAnimation;

std::function<void(void)> _callback;

void LedAnimatorClass::begin(std::function<void(void)> callback) {
  lastTime = millis();
  _callback = callback;
}

void LedAnimatorClass::handle() {
  unsigned long currTime = millis();
  if (currTime - lastTime >= 10) {
    animate();
    if (currTime - lastTime >= 20) lastTime = currTime; // overflow or animation disabled for longer period
    else lastTime += 10; // to make sure that average number of calculations per second is 100
    if(_callback) _callback();
  }
}

int LedAnimatorClass::animationCount() {
  return animations.GetSize();
}

int LedAnimatorClass::getValue(int id) {
  for(unsigned int i=0;i<animations.GetSize();i++) {
    if (animations[i].getId() == id) return animations[i].getValue();
  }
  return 0;
}

LedAnimation* LedAnimatorClass::ensureAnimation(int id) {
  for(unsigned int i=0;i<animations.GetSize();i++) {
    if (animations[i].getId() == id) return &animations[i];
  }
  LedAnimation result;
  result.setId(id);
  animations.Add(result);
  return &animations[animations.GetSize()-1];
}

void LedAnimatorClass::animate() {
  for(unsigned int i=0;i<animations.GetSize();i++) {
    if (!animations[i]._isTriggeredExternaly) animations[i].animate();
  }
}

LedAnimation* LedAnimatorClass::configStart(int id, bool isTriggeredExternaly) {
	currentConfigAnimation = ensureAnimation(id);
  currentConfigAnimation->_isTriggeredExternaly = isTriggeredExternaly;
  currentConfigAnimation->clear();
  return currentConfigAnimation;
}

void LedAnimatorClass::configAddStep(short target, unsigned short delay) {
	currentConfigAnimation->addStep(delay, target, 0, 0); // do not repeat that
}

void LedAnimatorClass::configAddRepeat(short commands, short count) {
  currentConfigAnimation->addStep(0, -1, -commands, count);
}

void LedAnimatorClass::configEnd() {
  currentConfigAnimation->addStep(6000, -1, 0, -1); // every minute jump to itself
  currentConfigAnimation->enable();
}

LedAnimatorClass LedAnimator;
