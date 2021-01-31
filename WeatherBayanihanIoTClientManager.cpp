#include "WeatherBayanihanIoTClientManager.h"
#include "Arduino.h"
#include <ESP8266HTTPClient.h>

ConfigurableSerial httpSerial;

void WeatherBayanihanIoTClientManager::Begin(boolean enabled) {
  _enabled = enabled;
  httpSerial.Out("+ WeatherBayanihanIoTClientManager ");
  httpSerial.Out("--- WeatherBayanihanIoTClientManager _enabled", _enabled);
}

void WeatherBayanihanIoTClientManager::ConfigureLogging(ConfigurableSerial configurableSerial) {
  httpSerial = configurableSerial;
  httpSerial.Out("-- WeatherBayanihanIoTClientManager ConfigureLogging");
}

boolean WeatherBayanihanIoTClientManager::Send(String deviceId, String temperature, String pressure, String humidity) {
  httpSerial.Out("-- WeatherBayanihanIoTClientManager Send");
  if (_enabled) {

    String payload = "{  \"at\": " + temperature + ", \"ap\": " + pressure + ", \"rh\": " + humidity + ", \"deviceId\": \"" + deviceId + "\" }";
    
    httpSerial.Out("-- WeatherBayanihanIoTClientManager Send serverAddress", serverAddress);
    httpSerial.Out("-- WeatherBayanihanIoTClientManager Send sslFingerPrint", sslFingerPrint);
    httpSerial.Out("-- WeatherBayanihanIoTClientManager Send deviceId", deviceId);
    httpSerial.Out("-- WeatherBayanihanIoTClientManager Send temperature", temperature);
    httpSerial.Out("-- WeatherBayanihanIoTClientManager Send pressure", pressure);
    httpSerial.Out("-- WeatherBayanihanIoTClientManager Send humidity", humidity);
    httpSerial.Out("-- WeatherBayanihanIoTClientManager Send payload", payload);
    
    HTTPClient httpClient;
    httpClient.begin(serverAddress, sslFingerPrint);
    httpClient.addHeader("Content-Type", "application/json");
    
    int http_response_code = httpClient.POST(payload);
    
    String http_response_payload = httpClient.getString();
    httpClient.end();
    
    httpSerial.Out("-- WeatherBayanihanIoTClientManager Send Complete - http_response_code", http_response_code);
    httpSerial.Out("-- WeatherBayanihanIoTClientManager Send Complete - http_response_payload", http_response_payload);

    return ((http_response_code == 202) ? true : false);
  }
  else {
    return false;
  }
}
