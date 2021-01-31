#ifndef SensorManager_h
#define SensorManager_h
#include "Arduino.h"
#include "ConfigurableSerial.h"
#include "dht.h"

class SensorManager {
  public:
    void Begin(int sclPIN, int sdaPIN, boolean enabled = true);
    void ProcessSensors();
    void ConfigureLogging(ConfigurableSerial configurableSerial);
    boolean Initialized = false;
    double Temperature = 0.0;
    double Humidity = 0.0;
    double Pressure = 0.0;
    boolean CheckSensors();
  private:
    String _prefix;
    boolean _enabled;
    int _sclPIN;
    int _sdaPIN;
};

#endif
