#include "HTTPManager.h"
#include "Arduino.h"
#include <ESP8266HTTPClient.h>

ConfigurableSerial httpSerial;

void HTTPManager::Begin(boolean enabled) {
  _enabled = enabled;
  httpSerial.Out("-- HTTPManager ");
  httpSerial.Out("--- HTTPManager _enabled", _enabled);
}

void HTTPManager::ConfigureClient(String serverAddress, String sslFingerPrint) {
  httpSerial.Out("--- HTTPManager ConfigureClient");
  _serverAddress = serverAddress;
  _sslFingerPrint = sslFingerPrint;

  httpSerial.Out("--- HTTPManager ConfigureClient _serverAddress", _serverAddress);
  httpSerial.Out("--- HTTPManager ConfigureClient _sslFingerPrint", _sslFingerPrint);
}

void HTTPManager::ConfigureLogging(ConfigurableSerial configurableSerial) {
  httpSerial = configurableSerial;
  httpSerial.Out("-- HTTPManager ConfigureLogging");
}

boolean HTTPManager::Send(String payload) {
  httpSerial.Out("-- HTTPManager Send");
  if (_enabled) {
    httpSerial.Out("-- HTTPManager Send _serverAddress", _serverAddress);
    httpSerial.Out("-- HTTPManager Send _sslFingerPrint", _sslFingerPrint);
    httpSerial.Out("-- HTTPManager Send payload", payload);
    
    HTTPClient httpClient;
    httpClient.begin(_serverAddress, _sslFingerPrint);
    httpClient.addHeader("Content-Type", "application/json");
    
    int http_response_code = httpClient.POST(payload);
    String http_response_payload = httpClient.getString();
    httpClient.end();

    httpSerial.Out("-- HTTPManager Send Complete - http_response_code", http_response_code);
    httpSerial.Out("-- HTTPManager Send Complete - http_response_payload", http_response_payload);

    return ((http_response_code == 202) ? true : false);
  }
  else {
    return false;
  }
}
