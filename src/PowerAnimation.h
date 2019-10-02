#ifndef POWERANIMATION_H
#define POWERANIMATION_H

#include "DynArray.h"

struct AnimStep {
  // static values:
	unsigned short stepTime; // in anim cycles
	short targetValue; // -1 - no change
  short nextStepRel; // relative index of next step (default: 1)
  short repeat; // -1 - infinite loop

  // dynamic values (changing over time)
  short _repeat;
};

class PowerAnimation {
  public:
    DynArray<AnimStep> _steps;
    bool _isTriggeredExternaly;

    void animate();
    int getId();
    void setId(int id);
    short getValue();
    unsigned short getStepIndex();
    unsigned int getStepTime();
    void clear();
    void enable();
    void addStep(unsigned short stepTime, short targetValue, short nextStepRel, short repeat);
  protected:
    bool _enabled;
    int _id;
    float _value;
    unsigned short _stepIndex;
    unsigned int _stepTime;
};

#endif
