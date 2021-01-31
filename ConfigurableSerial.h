#ifndef ConfigurableSerial_h
#define ConfigurableSerial_h
#include "Arduino.h"

class ConfigurableSerial {
  public:
    void Begin(String prefix, int rate, boolean enabled);
    void Out();
    void Out(String input);
    void Out(String input, bool newline);
    void Out(String input, int iinput);
    void Out(String input, String sinput);
    void Out(String input, long linput);
    void Out(String input, double dinput);
    boolean Initialized = false;
  private:
    String _prefix;
    boolean _enabled;
};

#endif
