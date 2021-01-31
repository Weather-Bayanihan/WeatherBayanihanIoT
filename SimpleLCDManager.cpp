#include "SimpleLCDManager.h"
#include "Arduino.h"

#define lcdScreen_address 0x20                //0x27
#define lcdScreen_columns 20                  //16 or 20
#define lcdScreen_rows 4                      //2 or 4

LiquidCrystal_I2C lcdScreen(lcdScreen_address, lcdScreen_columns, lcdScreen_rows);
ConfigurableSerial lSerial;

void SimpleLCDManager::Begin(String prefix, boolean enabled) {
  _prefix  = prefix;
  _enabled = enabled;

  if (_enabled) {
    lSerial.Out("+ SimpleLCDManager ");
    lSerial.Out("- SimpleLCDManager _enabled", _enabled);
    
    lSerial.Out("--- SimpleLCDManager lcdScreen_address", lcdScreen_address);
    lSerial.Out("--- SimpleLCDManager lcdScreen_columns", lcdScreen_columns);
    lSerial.Out("--- SimpleLCDManager lcdScreen_rows", lcdScreen_rows);
      
    lcdScreen.begin();
    lcdScreen.clear();
    lcdScreen.backlight();
    lcdScreen.createChar(1, customChar_wiFi);
    lcdScreen.createChar(2, customChar_wiFi_notConnected);
    lcdScreen.createChar(3, customChar_degrees);
    lcdScreen.createChar(4, customChar_percent);
    lcdScreen.createChar(5, customChar_clock);
    lcdScreen.createChar(6, customChar_up);
    lcdScreen.createChar(7, customChar_hp);
  }
}

void SimpleLCDManager::ConfigureLogging(ConfigurableSerial configurableSerial) {
  lSerial = configurableSerial;
  lSerial.Out("-- SimpleLCDManager ConfigureLogging");
}

void SimpleLCDManager::PrintText(int row, String input1, double input2) {
  PrintText(row, input1, String(input2));
}

void SimpleLCDManager::PrintText(int row, String input1, String input2) {
  PrintText(row, input1 + input2);
}

void SimpleLCDManager::PrintText(int row, String input) {
  if (_enabled) {
    lSerial.Out("-- SimpleLCDManager PrintText");
    lSerial.Out("--- PrintText - row", row);
    String _input = input.substring(0, lcdScreen_columns - 1) + " ";
    lSerial.Out("--- PrintText - input", _input);
    
    ClearRow(row);
    lcdScreen.setCursor(0, row);
    lcdScreen.print(_input);
  }
}

void SimpleLCDManager::PrintIcon(int row, int iconId) {
  if (_enabled) {
    lSerial.Out("-- SimpleLCDManager PrintIcon");
    lSerial.Out("--- PrintIcon - row", row);
    lSerial.Out("--- PrintIcon - iconId", iconId);
    lcdScreen.setCursor(19, row);
    lcdScreen.write(iconId);
  }
}
void SimpleLCDManager::ClearRow(int row) {
  lcdScreen.setCursor(0, row);
  lcdScreen.print("                    ");
}
