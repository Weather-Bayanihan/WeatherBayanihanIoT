#include "SensorManager.h"
#include "Arduino.h"
#include "Adafruit_BMP280.h"
#include "Adafruit_BME280.h"
#include <Wire.h>

#define DHT_PIN_SENSOR 0    //s   - D1 | Digital 1 = PIN 5, Digital 3 = PIN 0
#define BMP_PIN_SCL 14      //scl - D5
#define BMP_PIN_SDA 12      //sda - D6
#define BMP_PIN_CSB 13      //csb - D7
#define BMP_PIN_SDO 15      //sdo - D8
#define BME_ADDR_I2C 0x76   //0x76
#define BME_PIN_SCL 5       //scl - D5 = 14, D1 = 5
#define BME_PIN_SDA 4       //sda - D6 = 12, D2 = 4
#define DHT_ENABLED false   //! IMPORTANT !- TOGGLE DHT because D1 pin to be used by LCD
#define BMP_ENABLED false   //! IMPORTANT !- TOGGLE BMP and BME because of the SCL and SDA PINS on D5 and D6 respectively
#define BME_ENABLED true    //! IMPORTANT !- TOGGLE BMP and BME because of the SCL and SDA PINS on D5 and D6 respectively

ConfigurableSerial smSerial;

dht dhtSensor;
Adafruit_BMP280 bmpSensor(BMP_PIN_CSB, BMP_PIN_SDA, BMP_PIN_SDO, BMP_PIN_SCL);       //IF USE BMP
Adafruit_BME280 bmeSensor;                                                           //IF USE BME

//https://links2004.github.io/Arduino/d9/d4b/class_two_wire.html#a4c7daf378c06e5e72762e1bd3d5937b6
TwoWire BME_I2C_TWOWIRE;    //Example of using another instance of WIRE to utilize other PINS.

void SensorManager::Begin(boolean enabled) {
  _enabled = enabled;
  smSerial.Out("+ SensorManager ");
  if (!_enabled) {
    return;
  }
  
  smSerial.Out("- SensorManager Starting....");
  smSerial.Out("--- SensorManager _enabled", (_enabled ? "ENABLED" : "DISABLED"));
  smSerial.Out("--- SensorManager DHT_PIN_SENSOR", DHT_PIN_SENSOR);
  smSerial.Out("--- SensorManager BMP_PIN_SCL", BMP_PIN_SCL);
  smSerial.Out("--- SensorManager BMP_PIN_SDA", BMP_PIN_SDA);
  smSerial.Out("--- SensorManager BMP_PIN_CSB", BMP_PIN_CSB);
  smSerial.Out("--- SensorManager BMP_PIN_SDO", BMP_PIN_SDO);
  smSerial.Out("--- SensorManager BME_ADDR_I2C", BME_ADDR_I2C);
  smSerial.Out("--- SensorManager BME_PIN_SCL", BME_PIN_SCL);
  smSerial.Out("--- SensorManager BME_PIN_SDA", BME_PIN_SDA);
  smSerial.Out("--- SensorManager DHT_ENABLED", (DHT_ENABLED ? "ENABLED" : "DISABLED"));
  smSerial.Out("--- SensorManager BMP_ENABLED", (BMP_ENABLED ? "ENABLED" : "DISABLED"));
  smSerial.Out("--- SensorManager BME_ENABLED", (BME_ENABLED ? "ENABLED" : "DISABLED"));

  if (BMP_ENABLED) { //IF USE BMP
    if (!bmpSensor.begin()) {
      smSerial.Out("-- SensorManager bmpSensor: Error - Could not find a valid BMP sensor");
    } else {
      smSerial.Out("-- SensorManager bmpSensor: Started!");
    }
  }

  if (BME_ENABLED) {
    BME_I2C_TWOWIRE = TwoWire();
    BME_I2C_TWOWIRE.begin(BME_PIN_SDA, BME_PIN_SCL, 100000);
    if (!bmeSensor.begin(BME_ADDR_I2C, &BME_I2C_TWOWIRE)) { //IF USE BME
      smSerial.Out("-- SensorManager bmeSensor: Error - Could not find a valid BME sensor");
    } else {
      smSerial.Out("-- SensorManager bmeSensor: Started!");
    }
  }

}

void SensorManager::ConfigureLogging(ConfigurableSerial configurableSerial) {
  smSerial = configurableSerial;
  smSerial.Out("-- SensorManager ConfigureLogging");
}

void SensorManager::ProcessSensors() {
  if (!_enabled) {
    return;
  }

  smSerial.Out("-- SensorManager ProcessSensors");

  if (DHT_ENABLED) {
    smSerial.Out("-- SensorManager ProcessSensors - Use DHT11");
    dhtSensor.read11(DHT_PIN_SENSOR);
    Temperature = dhtSensor.temperature;
    Humidity = dhtSensor.humidity;
  }

  if (BMP_ENABLED) {
    smSerial.Out("-- SensorManager ProcessSensors - Use BMP180");
    Temperature = bmpSensor.readTemperature();
    Pressure = bmpSensor.readPressure() / 100;
  }

  if (BME_ENABLED) {
    smSerial.Out("-- SensorManager ProcessSensors - Use BME280");
    Temperature = bmeSensor.readTemperature();
    Humidity = bmeSensor.readHumidity();
    Pressure = bmeSensor.readPressure() / 100;
  }

  smSerial.Out("--- SensorManager - ProcessSensors - Current Temperature", Temperature);
  smSerial.Out("--- SensorManager - ProcessSensors -Current Humidity", Humidity);
  smSerial.Out("--- SensorManager - ProcessSensors - Current Pressure", Pressure);
}

boolean SensorManager::CheckSensors() {
  smSerial.Out("-- SensorManager CheckSensorMeasurements");
  if (!_enabled) {
    return false;
  }
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
