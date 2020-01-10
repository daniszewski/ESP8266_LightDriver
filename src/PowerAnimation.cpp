#include "PowerAnimation.h"

void PowerAnimation::animate() {
  if(!_enabled || _stepIndex>=_steps.size()) return;

  AnimStep *s = &_steps[_stepIndex];

  if (_stepTime) {
    if (_stepTime < UINT32_MAX) {
      if (s->targetValue>=0) _value += ((float)s->targetValue-_value) / (float)_stepTime;
      _stepTime--;
    }
  } else {
    while (_stepTime==0) {
      if (s->targetValue>=0) _value = s->targetValue; // write final value
      if (s->_repeat != 0) {
        if(s->_repeat > 0) s->_repeat--;
        _stepIndex += s->nextStepRel;
      } else {
        s->_repeat = s->repeat; // reset of dynamic repeat counter
        _stepIndex++; // next animation step
      }
      s = &_steps[_stepIndex]; // change current step
      _stepTime = s->stepTime; // get time from new step
    }
  }
}

short PowerAnimation::getValue() {
  return (short)_value;
}

unsigned short PowerAnimation::getStepIndex() {
  return _stepIndex;
}

unsigned int PowerAnimation::getStepTime() {
  return _stepTime;
}

int PowerAnimation::getId() {
  return _id;
}

void PowerAnimation::setId(int id) {
  _id = id;
}

void PowerAnimation::clear() {
  _enabled = false;
  _stepIndex = 0;
  _stepTime = 0;
  _steps.clear();
}

void PowerAnimation::enable() {
  _stepIndex = 0;
  AnimStep *s = &_steps[_stepIndex];
  _stepTime = s->stepTime;
  _enabled = true;
}

void PowerAnimation::addStep(unsigned int stepTime, short targetValue, short nextStepRel, short repeat) {
  _enabled = false;
  AnimStep s;
  s.stepTime = stepTime;
  s.targetValue = targetValue;
  s.nextStepRel = nextStepRel;
  s.repeat = repeat;
  s._repeat = repeat;

  _steps.push_back(s);
}


