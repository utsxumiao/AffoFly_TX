#ifndef AFFOFLY_TRANSMITTER_H_
#define AFFOFLY_TRANSMITTER_H_

extern uint8_t TX_MODE;
extern uint16_t TX_UNIQUE_ID;
extern RxConfig CURRENT_RX_CONFIG;
extern Bounce bounces[BUTTON_COUNT];

void Control_init();
void Control_initConfig();
void Control_initButtons();
void Control_resetData();
void Control_checkButtons();
void Control_getData();
uint16_t mapJoystickValues(uint16_t val, uint16_t lower, uint16_t middle, uint16_t upper, bool reverse);
void trimStick(bool increase);
void txModeInit();
void txModeProcess(uint32_t currentMillis);

#endif /* AFFOFLY_TRANSMITTER_H_ */
