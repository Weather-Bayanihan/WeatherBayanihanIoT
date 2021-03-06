#include "SensorManager.h"
#include "Arduino.h"
#include "Adafruit_BMP280.h"
#include "Adafruit_BME280.h"
#include "SFE_BMP180.h"
#include <Wire.h>

#define DHT_PIN_SENSOR 0      //dat - D1 | Digital 1 = PIN 5, Digital 3 = PIN 0
#define BMP_PIN_SCL 14        //scl - D5
#define BMP_PIN_SDA 12        //sda - D6
#define BMP_PIN_CSB 13        //csb - D7
#define BMP_PIN_SDO 15        //sdo - D8
#define BME_ADDR_I2C 0x76     //0x76

#define DHT_ENABLED false     //! IMPORTANT !- TOGGLE  - DHT because D1 pin to be used by LCD
#define BMP_ENABLED false     //! IMPORTANT !- TOGGLE - BMP 280 6 PINS
#define BME_ENABLED true      //! IMPORTANT !- TOGGLE - BME 4 PINS
#define BMPSFE_ENABLED false  //! IMPORTANT !- TOGGLE - BMP 180 4 PINS

ConfigurableSerial smSerial;

dht dhtSensor;
SFE_BMP180      bmpSensor_sfe;                                                       //IF USE SFE BME LIBRARY 
Adafruit_BMP280 bmpSensor(BMP_PIN_CSB, BMP_PIN_SDA, BMP_PIN_SDO, BMP_PIN_SCL);       //IF USE BMP, 6 PINS
Adafruit_BME280 bmeSensor;                                                           //IF USE ADAFRUIT BME LIBRARY 


//https://links2004.github.io/Arduino/d9/d4b/class_two_wire.html#a4c7daf378c06e5e72762e1bd3d5937b6
TwoWire BME_I2C_TWOWIRE;    //Example of using another instance of WIRE to utilize other PINS.

void SensorManager::Begin(int sclPIN, int sdaPIN, boolean enabled) {
  _enabled = enabled;
  _sclPIN = sclPIN;
  _sdaPIN = sdaPIN;

  smSerial.Out("+ SensorManager ");
  if (!_enabled) {
    return;
  }
  
  smSerial.Out("- SensorManager Starting....");
  smSerial.Out("--- SensorManager _enabled", String(_enabled));
  smSerial.Out("--- SensorManager DHT_PIN_SENSOR", DHT_PIN_SENSOR);
  smSerial.Out("--- SensorManager BMP_PIN_SCL", BMP_PIN_SCL);
  smSerial.Out("--- SensorManager BMP_PIN_SDA", BMP_PIN_SDA);
  smSerial.Out("--- SensorManager BMP_PIN_CSB", BMP_PIN_CSB);
  smSerial.Out("--- SensorManager BMP_PIN_SDO", BMP_PIN_SDO);
  smSerial.Out("--- SensorManager BME_ADDR_I2C", BME_ADDR_I2C);
  smSerial.Out("--- SensorManager sclPIN", _sclPIN);
  smSerial.Out("--- SensorManager sdaPIN", _sdaPIN);
  smSerial.Out("--- SensorManager DHT_ENABLED", String(DHT_ENABLED));
  smSerial.Out("--- SensorManager BMP_ENABLED", String(BMP_ENABLED));
  smSerial.Out("--- SensorManager BME_ENABLED", String(BME_ENABLED));

  if (BMPSFE_ENABLED) { //IF USE BMP180 SFE
    if (!bmpSensor_sfe.begin()) {
      smSerial.Out("-- SensorManager bmpSensor SFE: Error - Could not find a valid BMP SFE sensor");
    } else {
      smSerial.Out("-- SensorManager bmpSensor SFE: Started!");
    }
  }


  if (BMP_ENABLED) { //IF USE BMP
    if (!bmpSensor.begin()) {
      smSerial.Out("-- SensorManager bmpSensor: Error - Could not find a valid BMP sensor");
    } else {
      smSerial.Out("-- SensorManager bmpSensor: Started!");
    }
  }

  if (BME_ENABLED) {
    BME_I2C_TWOWIRE = TwoWire();
    BME_I2C_TWOWIRE.begin(_sdaPIN, _sclPIN, 100000);
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

  if (BMPSFE_ENABLED) {
    smSerial.Out("-- SensorManager ProcessSensors - Use BMP180 SFE");
    Temperature = bmpSensor_sfe.getTemperatureC(); //getTemperatureF //(C/F)
    Pressure = bmpSensor_sfe.getPressure();
  }

  if (DHT_ENABLED) {
    smSerial.Out("-- SensorManager ProcessSensors - Use DHT11");
    dhtSensor.read11(DHT_PIN_SENSOR);
    Temperature = dhtSensor.temperature;
    Humidity = dhtSensor.humidity;
  }

  if (BMP_ENABLED) {
    smSerial.Out("-- SensorManager ProcessSensors - Use BMP180");
    if(DHT_ENABLED == false){
      Temperature = bmpSensor.readTemperature();  
    }
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
