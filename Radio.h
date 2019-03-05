#ifndef RADIO_H_
#define RADIO_H_

void Radio_init();
void Radio_sendData(ControlData controlData);
void Radio_bind();
uint8_t findChannel();
uint32_t generateToken();
void bindRx(uint8_t channel, uint32_t token);

#endif /* RADIO_H_ */
