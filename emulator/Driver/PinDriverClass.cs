using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Threading.Tasks;

namespace ESP8266DriverEmu.Driver
{
    public interface IPin
    {
        string name {get; set; }
    }

    public class PinLight : IPin {
        public string name { get; set; }
        public char mode;
        public LightAnimation anim;
    }

    public class PinZero : IPin {
        public string name { get; set; }
    }

    public class PinSwitch : IPin {
        public string name { get; set; }
        public string onLow;
        public string onHigh;
        public char type;
        public bool hwState;
        public bool fnState; 
        public DateTime lastCloseTime;
    }

    public class PinDriverClass : IDisposable
    {
        string pinZero;
        int phaseStartTime;
        int phaseEndTime;
        int pulseLength;

        Dictionary<string, IPin> _pins = new Dictionary<string, IPin>();

        public Dictionary<string, IPin> Pins { get{return _pins;} }

        public void initZero(String pinName) {
            pinZero = pinName;
            _pins.Add(pinName, new PinZero() { name=pinName });
        }

        public void setPhaseStartTime(int time) {
            phaseStartTime = time;
        }

        public void setPhaseEndTime(int time) {
            phaseEndTime = time;
        }

        public void setPulseLength(int time) {
            pulseLength = time;
        }

        public void initPin(String pinName, String modeWord) { // mode: PWM, ZERO, OFF
            _pins.Add(pinName, new PinLight() { name=pinName, mode = modeWord[0] });
        }

        public void initSwitch(string pinName, string switchType, string onHigh, string onLow) {
            char type = switchType[0]; // 1, 2 or T/P/Z
            if (type > '9') type = 'T';
            _pins.Add(pinName, new PinSwitch() { name=pinName, onLow = onLow.ToLower(), onHigh = onHigh.ToLower(), type = type });
        }

        public void disablePin(String pinName) {
            _pins.Remove(pinName);
        }

        public void turnSwitch(String pinName, String state) {
            PinSwitch s = _pins[pinName] as PinSwitch;
            bool targetState = state=="1" ? true : state=="0" ? false : !s.fnState;
            onSwitchHandler(pinName, targetState);
        }


        public void setSwitchState(string pin, bool fnState) {
            PinSwitch s = _pins[pin] as PinSwitch;
            if (s.fnState != fnState) {
                s.fnState = fnState;
                DriverEmu.Instance.executeFile(@"storage\scripts\" + (fnState ? s.onHigh : s.onLow));
            }
        }

        public void onSwitchHandler(string pin, bool newState) {
            PinSwitch s = _pins[pin] as PinSwitch;
            if (s.type == '1') {
                if (newState) setSwitchState(pin, !s.fnState);
            } else if (s.type == '2') {
                setSwitchState(pin, newState);
            } else { // stair switch
                setSwitchState(pin, !s.fnState);
            }
        }

        public bool getSwitchValue(string pin) {
            return (_pins[pin] as PinSwitch).fnState;
        }

        public void setPinAnim(string pin, LightAnimation anim) {
            (_pins[pin] as PinLight).anim = anim;
        }

        public LightAnimation getPinAnim(string pin) {
            return (_pins[pin] as PinLight).anim;
        }

        public void Dispose()
        {
        }
    }
}
