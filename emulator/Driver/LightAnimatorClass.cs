using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Threading;
using System.Threading.Tasks;

namespace ESP8266DriverEmu.Driver
{
    public class LightAnimatorClass : IDisposable
    {
        public event EventHandler OnCallback;
        Dictionary<string, LightAnimation> animations = new Dictionary<string, LightAnimation>();
        LightAnimation currentConfigAnimation;

        Timer _timer;

        public LightAnimatorClass() {
            _timer = new Timer(_ =>  OnTick(), null, 10, Timeout.Infinite );
        }
        private void OnTick()
        {
            animate();
            OnCallback?.Invoke(this, EventArgs.Empty);
        }

        public int getValue(string id) {
            return animations[id].getValue();
        }

        public void animate() {
            foreach(var a in animations.Values) {
                if (!a._isTriggeredExternaly) a.animate();
            }
        }

        public LightAnimation ensureAnimation(string id) {
            if (!animations.TryGetValue(id, out LightAnimation result)) {
                result = new LightAnimation();
                animations.Add(id, result);
            }
            return result;
        }

        public LightAnimation configStart(string id, bool isTriggeredExternaly) {
            currentConfigAnimation = ensureAnimation(id);
            currentConfigAnimation._isTriggeredExternaly = isTriggeredExternaly;
            currentConfigAnimation.clear();
            return currentConfigAnimation;
        }

        public void configAddStep(int target, int delay) {
            currentConfigAnimation.addStep(delay, target, 0, 0); // do not repeat that
        }

        public void configAddRepeat(int commands, int count) {
            currentConfigAnimation.addStep(0, -1, -commands, count);
        }

        public void configEnd() {
            currentConfigAnimation.addStep(6000, -1, 0, -1); // every minute jump to itself
            currentConfigAnimation.enable();
        }

        public void Dispose() {}
    }
}
