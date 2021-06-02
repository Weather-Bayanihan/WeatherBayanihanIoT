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
    String sslFingerPrint = "65:BF:84:DF:3C:BC:7D:61:9A:1C:C1:BA:7F:11:65:4B:A7:30:95:9F";
    String serverAddress = "https://api.weather-bayanihan.ph/api/v1.0/Weather";
  
  private:
    boolean _enabled;
};

#endif
