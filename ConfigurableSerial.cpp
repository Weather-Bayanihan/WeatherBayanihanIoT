#include "ConfigurableSerial.h"
#include "Arduino.h"

void ConfigurableSerial::Begin(String prefix, int rate, boolean enabled) {
  _prefix  = prefix;
  _enabled = enabled;

  if (_enabled) {
    Serial.begin(rate);
    delay(3000);
    Out("-------------------------------");
    Out(prefix + " Starting..");
    Out("-------------------------------");
  }

  Initialized = true;
}

void ConfigurableSerial::Out(String input, bool newline) {
  if (_enabled && Initialized) {
    input = _prefix + ": " +  input;
    if (newline) {
      Serial.println(input);
    }
    else {
      Serial.print(input);
    }
  }
}

void ConfigurableSerial::Out(String input) {
  Out(input, true);
}

void ConfigurableSerial::Out() {
  Out("", true);
}

void ConfigurableSerial::Out(String input, int iinput) {
  if (_enabled && Initialized) {
    Out(input + " : " + String(iinput), true);
  }
}

void ConfigurableSerial::Out(String input, long linput) {
  if (_enabled && Initialized) {
    Out(input + " : " + String(linput), true);
  }
}

void ConfigurableSerial::Out(String input, double dinput) {
  if (_enabled && Initialized) {
    Out(input + " : " + String(dinput), true);
  }
}

void ConfigurableSerial::Out(String input, String sinput) {
  if (_enabled && Initialized) {
    Out(input + " : " + sinput, true);
  }
}
