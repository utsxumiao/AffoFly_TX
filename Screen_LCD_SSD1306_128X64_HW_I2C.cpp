#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "Screen.h"
#include <U8g2lib.h>

#ifdef SCREEN_LCD_SSD1306_128X64_HW_I2C

#define OLED_address   0x3C
U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

void Display_showWelcomeScreen(char* projectName, char* projectVersion) {}
void Display_showModeScreen(uint8_t txMode) {}
void Display_refreshMenu(char* title, MenuNodeItem* items, uint8_t count, uint8_t index, void (*displayFunc)(uint8_t*)) {}
void Display_showRxRename(uint8_t *lineCount) {}

#endif
