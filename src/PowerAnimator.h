#ifndef POWERANIMATOR_H
#define POWERANIMATOR_H

#include "Global.h"
#include "PowerAnimation.h"

typedef std::vector<PowerAnimation> PowerAnimationList;
//typedef DynArray<PowerAnimation> PowerAnimationList;

class PowerAnimatorClass {
  public:
    void begin(std::function<void(void)> callback);
    void handle();

    void animate();

    PowerAnimation* configStart(int id, bool isTriggeredExternaly);
    PowerAnimation* ensureAnimation(int id);
    void configAddStep(String value, String timeInfo);
    void configAddRepeat(short commands, short count);
    void configEnd();

    int getValue(int id);
  
  protected:
    unsigned long lastTime;
    PowerAnimationList animations;
};

extern PowerAnimatorClass PowerAnimator;

#endif
