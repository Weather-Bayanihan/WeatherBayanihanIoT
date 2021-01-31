#include "SensorManager.h"
#include "Arduino.h"
#include "Adafruit_BMP280.h"

#define DHT_PIN_SENSOR 0    //s   - D1 | Digital 1 = PIN 5, Digital 3 = PIN 0
#define BMP_PIN_SCL 14      //scl - D5
#define BMP_PIN_SDA 12      //sda - D6
#define BMP_PIN_CSB 13      //csb - D7
#define BMP_PIN_SDO 15      //sdo - D8

dht dhtSensor;
Adafruit_BMP280 bmpSensor(BMP_PIN_CSB, BMP_PIN_SDA, BMP_PIN_SDO, BMP_PIN_SCL);
ConfigurableSerial smSerial;

void SensorManager::Begin(boolean enabled) {
  _enabled = enabled;
  smSerial.Out("-- SensorManager ");
  smSerial.Out("--- SensorManager _enabled", _enabled);
  smSerial.Out("--- SensorManager DHT_PIN_SENSOR", DHT_PIN_SENSOR);
  smSerial.Out("--- SensorManager BMP_PIN_SCL", BMP_PIN_SCL);
  smSerial.Out("--- SensorManager BMP_PIN_SDA", BMP_PIN_SDA);
  smSerial.Out("--- SensorManager BMP_PIN_CSB", BMP_PIN_CSB);
  smSerial.Out("--- SensorManager BMP_PIN_SDO", BMP_PIN_SDO);
  
  if (_enabled) {
    if (!bmpSensor.begin()) {  
        smSerial.Out("-- SensorManager bmpSensor: Error - Could not find a valid bme sensor");
    }else{
        smSerial.Out("-- SensorManager bmpSensor: Started!");
    }
  }
}

void SensorManager::ConfigureLogging(ConfigurableSerial configurableSerial) {
  smSerial = configurableSerial;
  smSerial.Out("-- SensorManager ConfigureLogging");
}

void SensorManager::ProcessSensors() {
  if (!_enabled) {
    smSerial.Out("-- SensorManager Not Enabled");
    return;
  }

  smSerial.Out("-- SensorManager ProcessSensors");

  dhtSensor.read11(DHT_PIN_SENSOR);
  Temperature = dhtSensor.temperature;
  Humidity = dhtSensor.humidity;
  Pressure = bmpSensor.readPressure() / 100;
  
  smSerial.Out("--- Current Temperature", Temperature);
  smSerial.Out("--- Current Humidity", Humidity);
  smSerial.Out("--- Current Pressure", Pressure);
}

boolean SensorManager::CheckSensors() {
  smSerial.Out("-- SensorManager CheckSensorMeasurements");
  smSerial.Out("--- SensorManager CheckSensorMeasurements - Temperature", Temperature);
  smSerial.Out("--- SensorManager CheckSensorMeasurements - Humidity", Humidity);
  smSerial.Out("--- SensorManager CheckSensorMeasurements - Pressure", Pressure);
  
  if (Temperature != 0.0 && Humidity != 0) {
    smSerial.Out("-- SensorManager CheckSensorMeasurements - true");
    return true;
  }
  else {
    smSerial.Out("-- SensorManager CheckSensorMeasurements - false");
    return false;
  }
}
