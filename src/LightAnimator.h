#ifndef LIGHTANIMATOR_H
#define LIGHTANIMATOR_H

#include "Global.h"
#include "LightAnimation.h"

class LightAnimatorClass {
  public:
    void begin(std::function<void(void)> callback);
    void handle();

    void animate();

  	LightAnimation* configStart(int id, bool isTriggeredExternaly);
  	void configAddStep(short value, unsigned short delay);
    void configAddRepeat(short commands, short count);
  	void configEnd();

    int animationCount();
    int getValue(int id);
  
  protected:
    unsigned long lastTime;
    DynArray<LightAnimation> animations;
    LightAnimation* ensureAnimation(int id);
};

extern LightAnimatorClass LightAnimator;

#endif
