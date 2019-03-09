#ifndef AFFOFLY_TRANSMITTER_H_
#define AFFOFLY_TRANSMITTER_H_

extern uint8_t TX_MODE;
extern RxConfig CURRENT_RX_CONFIG;
extern Bounce bounces[BUTTON_COUNT];

void Control_init();
void Control_initConfig();
void Control_initButtons();
void Control_resetData();
void Control_checkButtons(uint32_t currentTime);
void Control_getData(uint32_t currentTime);
uint16_t mapJoystickValues(uint16_t val, uint16_t lower, uint16_t middle, uint16_t upper, bool reverse);
void trimStick(bool increase);
void txModeInit();
void txModeProcess(uint32_t currentTime);

#endif /* AFFOFLY_TRANSMITTER_H_ */
