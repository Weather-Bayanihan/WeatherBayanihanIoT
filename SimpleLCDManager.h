#ifndef SimpleLCDManager_h
#define SimpleLCDManager_h
#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "SimpleLCDManager.h"
#include "ConfigurableSerial.h"

class SimpleLCDManager {
  public:
    void Begin(String prefix, int sclPIN, int sdaPIN, boolean enabled);
    void ConfigureLogging(ConfigurableSerial configurableSerial);
    void PrintText(int row, String input);
    void PrintText(int row, String input1, String input2);
    void PrintText(int row, String input1, double input2);
    void ClearRow(int row);
    void PrintIcon(int row, int iconId);
    void ClearScreen();
  private:
    boolean _enabled;
    String _prefix;
    int _sclPIN;
    int _sdaPIN;
    
    //Credits to https://maxpromer.github.io/LCD-Character-Creator/
    byte customChar_wiFi[8] = {0x01, 0x01, 0x03, 0x03, 0x07, 0x07, 0x0F, 0x1F};
    byte customChar_wiFi_notConnected[8]  = {0x11, 0x0A, 0x04, 0x0A, 0x11, 0x04, 0x0C, 0x1C};
    byte customChar_degrees[8]  = {0x0C, 0x12, 0x12, 0x0C, 0x00, 0x00, 0x00, 0x00 };
    byte customChar_clock[8]  = {B00000, B01110, B10101, B10101, B10111, B10001, B01110, B00000};
    byte customChar_percent[8] = { 0x08, 0x14, 0x09, 0x02, 0x04, 0x0A, 0x15, 0x02 };
    byte customChar_up[8] = { 0x00, 0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x00 };
    byte customChar_hp[8] = { 0x10, 0x1C, 0x14, 0x03, 0x05, 0x07, 0x04, 0x04 };
};

#endif
