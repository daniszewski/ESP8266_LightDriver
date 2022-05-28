#ifndef POWERANIMATOR_H
#define POWERANIMATOR_H

#include <functional>
#include <Stream.h>
#include "PowerAnimation.h"

typedef std::vector<PowerAnimation> PowerAnimationList;

class PowerAnimatorClass {
  public:
    void begin();
    void handle();

    void setCallback(std::function<void(void)> callback);
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
  
  private:
    unsigned int parseTime(const String &time);
};

extern PowerAnimatorClass PowerAnimator;

#endif
