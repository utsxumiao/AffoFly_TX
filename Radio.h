#ifndef RADIO_H_
#define RADIO_H_

#ifdef SHOW_RATE
extern uint16_t RADIO_COUNT;
#endif

void Radio_init();
void Radio_sendData(ControlData controlData, uint32_t currentTime);
void Radio_bind();
uint8_t findChannel();
uint32_t generateToken();
void bindRx(uint8_t channel, uint32_t token);

#endif /* RADIO_H_ */
