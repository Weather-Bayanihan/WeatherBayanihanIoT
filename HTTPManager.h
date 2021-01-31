#ifndef HTTPManager_h
#define HTTPManager_h
#include "Arduino.h"
#include "ConfigurableSerial.h"
#include <ESP8266HTTPClient.h>

class HTTPManager {
  public:
    void Begin(boolean enabled);
    void ConfigureClient(String serverAddress, String sslFingerPrint);
    void ConfigureLogging(ConfigurableSerial configurableSerial);
    boolean Send(String payload);
    boolean Initialized = false;
  private:
    String _serverAddress;
    String _sslFingerPrint;
    boolean _enabled;
};

#endif
