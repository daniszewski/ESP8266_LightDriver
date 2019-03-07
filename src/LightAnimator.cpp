#include "LightAnimator.h"

LightAnimation *currentConfigAnimation;

std::function<void(void)> _callback;

void LightAnimatorClass::begin(std::function<void(void)> callback) {
  lastTime = millis();
  _callback = callback;
}

void LightAnimatorClass::handle() {
  unsigned long currTime = millis();
  if (currTime - lastTime >= 10) {
    animate();
    if (currTime - lastTime >= 20) lastTime = currTime; // overflow or animation disabled for longer period
    else lastTime += 10; // to make sure that average number of calculations per second is 100
    if(_callback) _callback();
  }
}

int LightAnimatorClass::animationCount() {
  return animations.GetSize();
}

int LightAnimatorClass::getValue(int id) {
  for(unsigned int i=0;i<animations.GetSize();i++) {
    if (animations[i].getId() == id) return animations[i].getValue();
  }
  return 0;
}

LightAnimation* LightAnimatorClass::ensureAnimation(int id) {
  for(unsigned int i=0;i<animations.GetSize();i++) {
    if (animations[i].getId() == id) return &animations[i];
  }
  LightAnimation result;
  result.setId(id);
  animations.Add(result);
  return &animations[animations.GetSize()-1];
}

void LightAnimatorClass::animate() {
  for(unsigned int i=0;i<animations.GetSize();i++) {
    if (!animations[i]._isTriggeredExternaly) animations[i].animate();
  }
}

LightAnimation* LightAnimatorClass::configStart(int id, bool isTriggeredExternaly) {
	currentConfigAnimation = ensureAnimation(id);
  currentConfigAnimation->_isTriggeredExternaly = isTriggeredExternaly;
  currentConfigAnimation->clear();
  return currentConfigAnimation;
}

void LightAnimatorClass::configAddStep(short target, unsigned short delay) {
	currentConfigAnimation->addStep(delay, target, 0, 0); // do not repeat that
}

void LightAnimatorClass::configAddRepeat(short commands, short count) {
  currentConfigAnimation->addStep(0, -1, -commands, count);
}

void LightAnimatorClass::configEnd() {
  currentConfigAnimation->addStep(6000, -1, 0, -1); // every minute jump to itself
  currentConfigAnimation->enable();
}

LightAnimatorClass LightAnimator;
