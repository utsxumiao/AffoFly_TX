#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "Display.h"

#ifdef DISPLAY_LCD_1602A_HW_I2C

void Display_refreshMenu(char* title, char menuItems[][MENU_ITEM_LENGTH], uint8_t count, uint8_t currentMenuIndex) {
  
}

#endif
