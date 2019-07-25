#include <Arduino.h>
#include <EwmaT.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define MV_PER_AMP 66
#define R1 30000.0
#define R2 7500.0
#define BATTERY_VOLTAGE_PIN A0
#define CHARGE_CURRENT_PIN A1
#define LOAD_CURRENT_PIN A2
#define I2C_ADDRESS 0x3C
#define RST_PIN -1
#define ACSoffset 2500

EwmaT<int> batteryRawFilter(1, 10);
int builtinLEDstatus = 0;
int batteryRaw;
int batteryFiltered;
int chargeRaw;
int loadRaw;
float batteryVoltage = 0;
uint32_t frame = 0;
SSD1306AsciiWire oled;

float loadCurrentVoltage = 0;
float loadCurrentAmps = 0;
float chargeCurrentVoltage = 0;
float chargeCurrentAmps = 0;


//------------------------------------------------------------------------------

float rawToVoltage(float raw) {
  return  ((raw * 5.0) / 1024.0) / (R2 / (R1 + R2));
}

float rawToVoltage(int raw) {
  return  ((raw * 5.0) / 1024.0) / (R2 / (R1 + R2));
}

void setup() {
  Wire.begin();
  Wire.setClock(400000L);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BATTERY_VOLTAGE_PIN, INPUT);
  pinMode(CHARGE_CURRENT_PIN, INPUT);
  pinMode(LOAD_CURRENT_PIN, INPUT);

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif
  // Call oled.setI2cClock(frequency) to change from the default frequency.

  oled.setFont(Adafruit5x7);
}

void drawStatus() {
  frame += 1;
  builtinLEDstatus = !builtinLEDstatus;
  oled.setRow(0);
  oled.setCol(0);
  oled.set2X();
  oled.print("STATUS: ");

  oled.print("AC");
  oled.clearToEOL();
  oled.println();

  oled.set1X();
  oled.print("Batt V: "); 
  oled.print(rawToVoltage(batteryFiltered)); 
  oled.print("V (");
  oled.print(batteryRaw);
  oled.print(")");
  oled.clearToEOL(); 
  oled.println();

  oled.print("CHG: ");
  oled.print(chargeCurrentAmps);
  oled.print("A @ ");
  oled.print(chargeCurrentVoltage/1000.0);
  oled.print("V (");
  oled.print(chargeRaw); 
  oled.print(")"); 
  oled.clearToEOL(); 
  oled.println();


  oled.print("LOAD: ");
  oled.print(loadCurrentAmps);
  oled.print("A @ ");
  oled.print(loadCurrentVoltage/1000.0);
  oled.print("V (");
  oled.print(loadRaw); 
  oled.print(")"); 
  oled.clearToEOL(); 
  oled.println();


  oled.setCursor(oled.displayWidth() - 5, oled.displayRows() - 1);
  oled.print(builtinLEDstatus ? "." : " ");
}

//------------------------------------------------------------------------------
void loop() {
  batteryRaw = analogRead(BATTERY_VOLTAGE_PIN);
  batteryFiltered = batteryRawFilter.filter(batteryRaw);
  chargeRaw = analogRead(CHARGE_CURRENT_PIN);
  loadRaw = analogRead(LOAD_CURRENT_PIN);
  
  chargeCurrentVoltage = (chargeRaw / 1024.0) * 5000; // Gets you mV
  chargeCurrentAmps = ((chargeCurrentVoltage - ACSoffset) / MV_PER_AMP);
  
  loadCurrentVoltage = (loadRaw / 1024.0) * 5000; // Gets you mV
  loadCurrentAmps = ((loadCurrentVoltage - ACSoffset) / MV_PER_AMP);
 
  drawStatus();
  delay(500);
}
