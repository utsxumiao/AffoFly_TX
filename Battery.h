#ifndef BATTERY_H_
#define BATTERY_H_

#ifdef V_BAT

extern uint16_t BATTERY_VOLTAGE;
extern bool LOW_VOLTAGE;

void Battery_read(uint32_t currentTime);
void setLowVoltageAlarm(uint16_t voltage);
uint16_t readVoltage();

#endif

#endif /* BATTERY_H_ */
