#ifndef POWERANIMATION_H
#define POWERANIMATION_H

#include <stdint.h>
#include <vector>

struct AnimStep {
  // static values:
	unsigned int stepTime; // in anim cycles (1/100 of sec), the range is about 1.36 years
	short targetValue; // -1 - no change
  short nextStepRel; // relative index of next step (default: 1)
  short repeat; // -1 - infinite loop

  // dynamic values (changing over time)
  short _repeat;
};

typedef std::vector<AnimStep> AnimStepList;

class PowerAnimation {
  public:
    AnimStepList _steps;
    bool _isTriggeredExternaly;

    void animate();
    int getId();
    void setId(int id);
    short getValue();
    unsigned short getStepIndex();
    unsigned int getStepTime();
    void clear();
    void enable();
    void addStep(unsigned int stepTime, short targetValue, short nextStepRel, short repeat);
  protected:
    bool _enabled;
    int _id;
    float _value;
    unsigned short _stepIndex;
    unsigned int _stepTime;
};

#endif
