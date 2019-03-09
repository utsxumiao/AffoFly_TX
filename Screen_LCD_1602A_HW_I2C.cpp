#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "Screen.h"

#ifdef SCREEN_LCD_1602A_HW_I2C

void Display_showWelcomeScreen(char* projectName, char* projectVersion) {}
void Display_showModeScreen(uint8_t txMode) {}
void Display_refreshMenu(char* title, MenuNodeItem* items, uint8_t count, uint8_t index, void (*displayFunc)(uint8_t*)) {}
void Display_showRxRename(uint8_t *lineCount) {}

#endif
