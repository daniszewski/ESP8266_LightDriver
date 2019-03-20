using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Threading.Tasks;

namespace ESP8266DriverEmu.Driver
{
    public class AnimStep 
    {
        public int stepTime; // in anim cycles
        public int targetValue; // -1 - no change
        public int nextStepRel; // relative index of next step (default: 1)
        public int repeat; // -1 - infinite loop

        // dynamic values (changing over time)
        public int  _stepTime;
        public int _repeat;
    }

    public class LightAnimation
    {
        bool _enabled;
        int _stepIndex;
        float _value;

        public bool _isTriggeredExternaly;

        List<AnimStep> _steps = new List<AnimStep>();


        public void animate() {
            if(!_enabled || _stepIndex>=_steps.Count) return;

            AnimStep s = _steps[_stepIndex];

            if (s._stepTime!= 0) {
                if (s.targetValue>=0) _value += ((float)s.targetValue-_value) / (float)s._stepTime;
                s._stepTime--;
            } else {
                while (s._stepTime==0) {
                    s._stepTime = s.stepTime;
                    if (s.targetValue>=0) _value = s.targetValue;
                    if (s._repeat != 0) {
                        if(s._repeat > 0) s._repeat--;
                        _stepIndex += s.nextStepRel;
                    } else {
                        s._repeat = s.repeat;
                        _stepIndex++;
                    }
                    s = _steps[_stepIndex];
                }
            }
        }

        public short getValue() {
            return (short)_value;
        }

        public void clear() {
            _enabled = false;
            _stepIndex = 0;
            _steps.Clear();
        }

        public void enable() {
            _stepIndex = 0;
            _enabled = true;
        }

        public void addStep(int stepTime, int targetValue, int nextStepRel, int repeat) {
            _enabled = false;
            AnimStep s = new AnimStep();
            s.stepTime = stepTime;
            s.targetValue = targetValue;
            s.nextStepRel = nextStepRel;
            s.repeat = repeat;
            s._stepTime = stepTime;
            s._repeat = repeat;

            _steps.Add(s);
        }
    }
}


