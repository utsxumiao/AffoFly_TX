#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Buzzer.h"
#include "Battery.h"

#ifdef V_BAT

float BATTERY_VOLTAGE = 0;
bool LOW_VOLTAGE = false;
static const uint32_t batteryReadInterval = 10000000;
uint32_t previousBatteryReadTime = 0;

// set to your voltage divider resistor values
// voltage is taken on R2
uint32_t voltageDividerR1 = 200000;
uint32_t voltageDividerR2 = 10000;
// if voltage is off, use this value as percentage to shift up or down
// theoretically you can leave it "0" if your resistor is acurate enough
int8_t adjustment = 2;

void Battery_init() {
  // Was trying to do a dummy reading on the pin so it will be saturated for next reading
  // But since we need to switch voltage reference constantly, this becomes redundent
  // See readVoltage() function below
  //analogRead(V_BAT_PIN);
}

void Battery_read(uint32_t currentTime) {
  if (currentTime - previousBatteryReadTime >= batteryReadInterval || previousBatteryReadTime == 0) {
    previousBatteryReadTime = currentTime;
    BATTERY_VOLTAGE = readVoltage();
#ifdef DEBUG
    Serial.print("V_BAT: "); Serial.println(BATTERY_VOLTAGE);
#endif
    setLowVoltageAlarm(BATTERY_VOLTAGE);
  }
}

void setLowVoltageAlarm(uint16_t voltage) {
  if (voltage <= V_BAT_ALARM_VOLTAGE) {
    LOW_VOLTAGE = true;
#ifdef BUZZER
    BuzzerBeepPattern buzzerPattern = lowVoltage;
    Buzzer_start(buzzerPattern);
#endif 
  }
}

float readVoltage() {
  float resolutionVoltage = 0.00107422; // AREF(1.1v) / 1024
  analogReference(INTERNAL);
  uint16_t vReading = analogRead(V_BAT_PIN);
  delay(5); //Based on my testing 5ms the reading reaches stable state
  vReading = analogRead(V_BAT_PIN);
  analogReference(DEFAULT);
  float voltageBattery = (vReading * resolutionVoltage * ((voltageDividerR1 + voltageDividerR2) / voltageDividerR2) * (100 + adjustment)) / 100;
  return voltageBattery;
}

#endif
