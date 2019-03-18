#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "Screen.h"

#ifdef SCREEN_LCD_1602A_HW_I2C

void Screen_showWelcomeScreen(char* projectName, char* projectVersion) {}
void Screen_showModeScreen(uint8_t txMode) {}
void Screen_showMenu(char* title, MenuNodeItem* items, uint8_t count, uint8_t index, void (*displayFunc)(uint8_t*)) {}
void Screen_showMenuRxRename(uint8_t *lineCount) {}
void Screen_showControlScreen(ControlData controlData, RateData rateData, bool trimming, uint8_t trimStickIndex, uint32_t currentTime, bool forceExecute) {}
void Screen_showRadioBindingScreen(uint8_t channel, bool tokenIsReady, bool bound) {}

#endif
