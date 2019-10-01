#ifndef POWERANIMATOR_H
#define POWERANIMATOR_H

#include "Global.h"
#include "PowerAnimation.h"

class PowerAnimatorClass {
  public:
    void begin(std::function<void(void)> callback);
    void handle();

    void animate();

    PowerAnimation* configStart(int id, bool isTriggeredExternaly);
    void configAddStep(short value, unsigned short delay);
    void configAddRepeat(short commands, short count);
    void configEnd();

    int animationCount();
    int getValue(int id);
  
  protected:
    unsigned long lastTime;
    DynArray<PowerAnimation> animations;
    PowerAnimation* ensureAnimation(int id);
};

extern PowerAnimatorClass PowerAnimator;

#endif
