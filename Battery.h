#ifndef BATTERY_H_
#define BATTERY_H_

#ifdef V_BAT

extern float BATTERY_VOLTAGE;
extern bool LOW_VOLTAGE;

void Battery_init();
void Battery_read(uint32_t currentTime);
void setLowVoltageAlarm(uint16_t voltage);
float readVoltage();

#endif

#endif /* BATTERY_H_ */
