#ifndef WeatherBayanihanIoTClientManager_h
#define WeatherBayanihanIoTClientManager_h
#include "Arduino.h"
#include "ConfigurableSerial.h"
#include <ESP8266HTTPClient.h>

class WeatherBayanihanIoTClientManager {
  public:
    void Begin(boolean enabled);
    void ConfigureLogging(ConfigurableSerial configurableSerial);
    boolean Send(String deviceId, String temperature, String pressure, String humidity);
    boolean Initialized = false;
    String sslFingerPrint = "F2:D3:FE:3C:D0:0C:8C:B9:D9:CF:91:1A:8E:33:87:A0:0D:42:49:50";
    String serverAddress = "https://api.weather-bayanihan.ph/api/v1.0/Weather";
  
  private:
    boolean _enabled;
};

#endif
