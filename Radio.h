#ifndef RADIO_H_
#define RADIO_H_

void Radio_init();
void Radio_sendData(ControlData controlData);
void Radio_bind();
uint8_t findChannel();
char* generateToken();
void bindRx(uint8_t channel, char* token);

#endif /* RADIO_H_ */
