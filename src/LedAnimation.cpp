#include "LedAnimation.h"

void LedAnimation::animate() {
  if(!_enabled || _stepIndex>=_steps.GetSize()) return;

  AnimStep *s = &_steps[_stepIndex];

  if (s->_stepTime) {
    if (s->targetValue>=0) _value += ((float)s->targetValue-_value) / (float)s->_stepTime;
    s->_stepTime--;
  } else {
    while (s->_stepTime==0) {
      s->_stepTime = s->stepTime;
      if (s->targetValue>=0) _value = s->targetValue;
      if (s->_repeat != 0) {
        if(s->_repeat > 0) s->_repeat--;
        _stepIndex += s->nextStepRel;
      } else {
        s->_repeat = s->repeat;
        _stepIndex++;
      }
      s = &_steps[_stepIndex];
    }
  }
}

short LedAnimation::getValue() {
  return (short)_value;
}

int LedAnimation::getId() {
  return _id;
}

void LedAnimation::setId(int id) {
  _id = id;
}

void LedAnimation::clear() {
  _enabled = false;
  _stepIndex = 0;
  _steps.Clear();
}

void LedAnimation::enable() {
  _stepIndex = 0;
  _enabled = true;
}

void LedAnimation::addStep(unsigned short stepTime, short targetValue, short nextStepRel, short repeat) {
  _enabled = false;
  AnimStep s;
  s.stepTime = stepTime;
  s.targetValue = targetValue;
  s.nextStepRel = nextStepRel;
  s.repeat = repeat;
  s._stepTime = stepTime;
  s._repeat = repeat;

  _steps.Add(s);
}


