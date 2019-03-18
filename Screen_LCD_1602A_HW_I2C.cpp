#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "Screen.h"

#ifdef SCREEN_LCD_1602A_HW_I2C

bool controlScreenForceRefresh = false;

void Screen_showWelcomeScreen(char* projectName, char* projectVersion) {}
void Screen_showModeScreen(uint8_t txMode) {}
void Screen_showMenu(char* title, MenuNodeItem* items, uint8_t count, uint8_t index, void (*displayFunc)(uint8_t*)) {}
void Screen_showMenuRxRename(uint8_t *lineCount) {}

#endif
