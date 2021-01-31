//// Weather, Humidity, Temperature, Arduino, LCD, IoT, ESP8266
//// https://www.weather-bayanihan.ph/
//// https://github.com/weather-bayanihan
//// by tulungan[@]weather-bayanihan.ph

#include "Arduino.h"
#include "dht.h"
#include "ConfigurableSerial.h"
#include "SimpleWifiManager.h"
#include "SimpleLCDManager.h"
#include "SensorManager.h"
#include "HTTPManager.h"

#define global_projectName "WeatherBayanihanIoT"  //WeatherBayanihanIoT
#define global_serial_loggingEnabled true         //true
#define global_serial_loggingRate 115200          //115200 or 9600

#define global_PIN_SCL 5                          //scl - D1 = 5 (Default) or D5 = 14 (Custom)
#define global_PIN_SDA 4                          //sda - D2 = 4 (Default) or D6 = 12 (Custom)

#define global_wifi_enabled false                  //true
#define global_wifi_blocking_delay 3000           //3000
#define global_wifi_nonblocking_interval 3000     //3000
#define global_wifi_enablereconfiguration false    //true
#define global_wifilcd_nonblocking_interval 15000 //15000
#define global_lcd_enabled true                   //true
#define global_sensor_enabled true                //true
#define global_sensor_blocking_delay 3000         //3000
#define global_sensor_nonblocking_interval 15000  //15000
#define global_http_enabled false
#define global_http_blocking_delay 3000
#define global_http_nonblocking_interval 1800000  //15000
                                                  //1800000 // <-- 30 Mins
                                                  //3600000 // <-- 60 Mins
                                                  
#define global_http_sslFingerPrint "F2:D3:FE:3C:D0:0C:8C:B9:D9:CF:91:1A:8E:33:87:A0:0D:42:49:50"
#define global_http_serverAddress "https://api.weather-bayanihan.ph/api/v1.0/Weather"

ConfigurableSerial serialLogger;
SimpleWifiManager simpleWifiManager;
SimpleLCDManager simpleLCDManager;
SensorManager sensorManager;
HTTPManager httpManager;

void setup() {
  serialLogger.Begin(global_projectName, global_serial_loggingRate, global_serial_loggingEnabled);
  serialLogger.Out(global_projectName);
  serialLogger.Out("+ Global Setup Started!");

  simpleLCDManager.ConfigureLogging(serialLogger);
  simpleLCDManager.Begin(global_projectName, global_lcd_enabled);
  simpleLCDManager.PrintText(1, global_projectName);
  simpleLCDManager.PrintText(2, "Starting..");
  simpleLCDManager.PrintIcon(2, 5);

    
  simpleLCDManager.PrintText(2, "Starting WiFi.. ");
  simpleWifiManager.ConfigureLogging(serialLogger);
  simpleWifiManager.ConfigureDelays(global_wifi_blocking_delay, global_wifi_nonblocking_interval);
  simpleWifiManager.ConfigureProjectName(global_projectName);
  simpleWifiManager.ConfigureWiFiReconfiguration(global_wifi_enablereconfiguration);
  simpleWifiManager.Begin(global_wifi_enabled);

  simpleLCDManager.PrintText(2, "Starting Sensors.. ");
  sensorManager.ConfigureLogging(serialLogger);
  sensorManager.Begin(global_sensor_enabled);

  simpleLCDManager.PrintText(2, "Starting IoT.. ");
  httpManager.ConfigureLogging(serialLogger);
  httpManager.Begin(global_http_enabled);

  simpleLCDManager.ClearRow(1);
  simpleLCDManager.ClearRow(2);
  simpleLCDManager.ClearRow(3);
  serialLogger.Out("- Setup Complete!");

  ProcessSensors();
}

long wifi_loopPreviousTime = 0;
long sensor_loopPreviousTime = 0;
long http_loopPreviousTime = 0;

void loop() {

  simpleWifiManager.ProcessConnections();

  unsigned long sensor_loopCurrentTime = millis();
  unsigned long wifi_loopCurrentTime = millis();
  unsigned long http_loopCurrentTime = millis();

  if (wifi_loopCurrentTime - wifi_loopPreviousTime > global_wifilcd_nonblocking_interval) {
    wifi_loopPreviousTime = wifi_loopCurrentTime;

    simpleLCDManager.PrintText(0, simpleWifiManager.CheckConnectionName());
    simpleLCDManager.PrintIcon(0, simpleWifiManager.CheckConnection() ? 1 : 2);
  }

  if (sensor_loopCurrentTime - sensor_loopPreviousTime > global_sensor_nonblocking_interval) {
    sensor_loopPreviousTime = sensor_loopCurrentTime;
    
    ProcessSensors();
    
  }

  if (http_loopCurrentTime - http_loopPreviousTime > global_http_nonblocking_interval) {
    http_loopPreviousTime = http_loopCurrentTime;
    if (simpleWifiManager.CheckConnection() && sensorManager.CheckSensors()) {
      httpManager.ConfigureClient(global_http_serverAddress, global_http_sslFingerPrint);

      String http_device_stationId = simpleWifiManager.GetStationId();
      if(http_device_stationId != "") {
        String http_manager_payload = "{  \"at\": " + String(sensorManager.Temperature) + ", \"ap\": " + String(sensorManager.Pressure) + ", \"rh\": " + String(sensorManager.Humidity) + ", \"deviceId\": \"" + String(http_device_stationId) + "\" }";
        boolean http_manager_response = httpManager.Send(http_manager_payload);
      }
     else {
        serialLogger.Out("- http_device_stationId is not yet configured. Skipping Sending");
      } 
    }
    else {
      serialLogger.Out("- Connection is out or Sensors are out. Skipping Sending");
    }
  }
}

void ProcessSensors() {
  sensorManager.ProcessSensors();
  if (sensorManager.CheckSensors()) {
    simpleLCDManager.PrintText(1, "Temperature:  ", sensorManager.Temperature);
    simpleLCDManager.PrintIcon(1, 3); //Degrees
    simpleLCDManager.PrintText(2, "Humidity:     ", sensorManager.Humidity);
    simpleLCDManager.PrintIcon(2, 4); //Percent
    simpleLCDManager.PrintText(3, "Pressure:   ", sensorManager.Pressure);
    simpleLCDManager.PrintIcon(3, 7); //hPa
  }
}
