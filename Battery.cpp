#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Buzzer.h"
#include "Battery.h"

#ifdef V_BAT

uint16_t BATTERY_VOLTAGE = 0;
bool LOW_VOLTAGE = false;
static const uint32_t batteryReadInterval = 10000000;
uint32_t previousBatteryReadTime = 0;

void Battery_read(uint32_t currentTime) {
  if (currentTime - previousBatteryReadTime >= batteryReadInterval || previousBatteryReadTime == 0) {
    previousBatteryReadTime = currentTime;
    //TODO: Battery read and conversion logic
    uint16_t voltage = readVoltage();

    BATTERY_VOLTAGE = voltage;
    setLowVoltageAlarm(voltage);
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

uint16_t readVoltage() {
  return 340;
}

#endif
