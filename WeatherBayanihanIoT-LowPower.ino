//// Weather Bayanihan
//// Humidity, Temperature, Pressure, Arduino, IoT, ESP8266
//// https://www.weather-bayanihan.ph/
//// https://github.com/weather-bayanihan
//// https://www.facebook.com/WeatherBayanihan
//// by tulungan[@]weather-bayanihan.ph

#include "Arduino.h"
#include "dht.h"
#include "ConfigurableSerial.h"
#include "SimpleWifiManager.h"
#include "SimpleLCDManager.h"
#include "SensorManager.h"
#include "WeatherBayanihanIoTClientManager.h"

#define global_projectName "WeatherBayanihanIoT"  //WeatherBayanihanIoT
#define global_serial_loggingEnabled true         //true
#define global_serial_loggingRate 115200          //115200 or 9600 (ESP8266 vs ARDUINO UNO/ETC)

#define global_PIN_SCL 5                          //scl - D1 = 5 (Default) or D5 = 14 (Custom)
#define global_PIN_SDA 4                          //sda - D2 = 4 (Default) or D6 = 12 (Custom)

#define global_wifi_enabled true                  //true
#define global_wifi_blocking_delay 3000           //3000
#define global_wifi_nonblocking_interval 3000     //3000
#define global_wifi_enablereconfiguration false   //true
#define global_wifilcd_nonblocking_interval 15000 //15000
#define global_lcd_enabled false                  //true
#define global_sensor_enabled true                //true
#define global_sensor_blocking_delay 3000         //3000
#define global_sensor_nonblocking_interval 600000 //15000 = 15 Secs (Debug), 600000 = 10 Mins
#define global_http_enabled true                  //ALLOW IOT TO SEND
#define global_http_blocking_delay 3000
#define global_http_nonblocking_interval 1800000  //15000
//RATE OF SENDING VIA IOT
//15000 = 15 Secs (Debug), 600000 = 10 Mins, 1800000 = 30 Mins, 3600000 = 60 Mins

ConfigurableSerial serialLogger;
SimpleWifiManager simpleWifiManager;
SimpleLCDManager simpleLCDManager;
SensorManager sensorManager;
WeatherBayanihanIoTClientManager weatherBayanihanIoTClientManager;

long wifi_loopPreviousTime = 0;
long sensor_loopPreviousTime = 0;
long http_loopPreviousTime = 0;

void setup() {g
  serialLogger.Begin(global_projectName, global_serial_loggingRate, global_serial_loggingEnabled);
  serialLogger.Out(global_projectName);
  serialLogger.Out("+ Global Setup Started!");

  simpleWifiManager.ConfigureLogging(serialLogger);
  simpleWifiManager.Begin(global_wifi_enabled);
  simpleWifiManager.wiFi_station_id = "GUID-STATION-ID";
  
  boolean wiFiConnectWithRetry_Success = simpleWifiManager.wiFiConnectWithRetry("WIFISSID", "WIFIPASSWORD", 5, 6000);

  if (wiFiConnectWithRetry_Success) {
    serialLogger.Out("-- wiFiConnectWithRetry Success");

    sensorManager.ConfigureLogging(serialLogger);
    sensorManager.Begin(global_PIN_SCL, global_PIN_SDA, global_sensor_enabled);

    weatherBayanihanIoTClientManager.ConfigureLogging(serialLogger);
    weatherBayanihanIoTClientManager.Begin(global_http_enabled);

    ProcessSensors(); 
    SendData();                   

  }
  else {
    serialLogger.Out("-- wiFiConnectWithRetry Fail ");
  }

  serialLogger.Out("-- + Going to Deep Sleep!");
  ESP.deepSleep(1800e6);              //in microseconds / e6
}


void loop() {}

void loop_old() {

  //simpleWifiManager.ProcessConnections();

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
    SendData();
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

void SendData() {
  if (simpleWifiManager.CheckConnection() && sensorManager.CheckSensors()) {
    String http_device_stationId = simpleWifiManager.GetStationId();
    if (http_device_stationId != "") {

      String _temperature = String(sensorManager.Temperature);
      String _pressure = String(sensorManager.Pressure);
      String _humidity = String(sensorManager.Humidity);

      boolean http_manager_response = weatherBayanihanIoTClientManager.Send(http_device_stationId, _temperature , _pressure , _humidity);
    }
    else {
      serialLogger.Out("- http_device_stationId is not yet configured. Skipping Sending");
    }
  }
  else {
    serialLogger.Out("- Connection is out or Sensors are out. Skipping Sending");
  }
}
