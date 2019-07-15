#include <EwmaT.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

#define R1 30000.0
#define R2 7500.0
#define BATTERY_VOLTAGE_PIN A0
#define I2C_ADDRESS 0x3C
#define RST_PIN -1

EwmaT<int> batteryRawFilter(1, 10);
int builtinLEDstatus = 0;
int batteryRaw;
float batteryVoltage = 0;
uint32_t frame = 0;
SSD1306AsciiAvrI2c oled;
//------------------------------------------------------------------------------

float rawToVoltage(float raw) {
  return  ((raw * 5.0) / 1024.0) / (R2 / (R1 + R2));
}

float rawToVoltage(int raw) {
  return  ((raw * 5.0) / 1024.0) / (R2 / (R1 + R2));
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BATTERY_VOLTAGE_PIN, INPUT);

  Serial.begin(9600);
  Serial.print("Beginning initialiation... ");

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif
  // Serial.print("OLED initialized! ");

  // Call oled.setI2cClock(frequency) to change from the default frequency.

  oled.setFont(Adafruit5x7);
  Serial.println("Starting!");
}

void drawStatus() {
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
  oled.print(rawToVoltage(batteryRaw)); 
  oled.print("V"); 
  oled.print(" ("); 
  oled.print(batteryRaw);
  oled.print(")");
  oled.clearToEOL(); 
  oled.println();
  digitalWrite(LED_BUILTIN, builtinLEDstatus);
}

//------------------------------------------------------------------------------
void loop() {
  batteryRaw = batteryRawFilter.filter(analogRead(BATTERY_VOLTAGE_PIN));
  drawStatus();
  delay(200);
}
