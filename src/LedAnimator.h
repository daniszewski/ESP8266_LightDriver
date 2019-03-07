#ifndef LEDANIMATOR_H
#define LEDANIMATOR_H

#include "Global.h"
#include "LedAnimation.h"

class LedAnimatorClass {
  public:
    void begin(std::function<void(void)> callback);
    void handle();

    void animate();

  	LedAnimation* configStart(int id, bool isTriggeredExternaly);
  	void configAddStep(short value, unsigned short delay);
    void configAddRepeat(short commands, short count);
  	void configEnd();

    int animationCount();
    int getValue(int id);
  
  protected:
    unsigned long lastTime;
    DynArray<LedAnimation> animations;
    LedAnimation* ensureAnimation(int id);
};

extern LedAnimatorClass LedAnimator;

#endif
