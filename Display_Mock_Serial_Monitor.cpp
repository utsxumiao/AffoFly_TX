#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "Display.h"

#ifdef DISPLAY_SERIAL_MONITOR

const uint8_t SCREEN_WIDTH = 30;
const uint8_t SCREEN_LINES = 8;

void Display_showWelcomeScreen(char* projectName, char* projectVersion) {
  Serial.println("================================");
  Serial.println("=                              =");
  Serial.println("=                              =");
  Serial.println("=                              =");
  Serial.print("=          "); Serial.print(projectName); Serial.println("          =");
  Serial.print("=            "); Serial.print(projectVersion); Serial.println("            =");
  Serial.println("=                              =");
  Serial.println("=                              =");
  Serial.println("=                              =");
  Serial.println("================================");
}

void Display_showModeScreen(uint8_t txMode) {
  for (uint8_t i = 0; i < 10; i++) {
    Serial.println();
  }
  Serial.println("================================");
  Serial.println("=                              =");
  Serial.println("=                              =");
  Serial.println("=                              =");
  switch (txMode) {
    case MODE_CONTROL:
      Serial.println("=           CONTROL            =");
      break;
    case MODE_SIMULATOR:
      Serial.println("=           SIMULATOR          =");
      break;
    case MODE_TRAINER:
      Serial.println("=           TRAINER            =");
      break;
    case MODE_STUDENT:
      Serial.println("=           STUDENT            =");
      break;
    case MODE_SETTING:
      Serial.println("=           SETTING            =");
      break;
    case MODE_MENU:
      Serial.println("=            MENU              =");
      break;
    default:
      break;
  }
  Serial.println("=                              =");
  Serial.println("=                              =");
  Serial.println("=                              =");
  Serial.println("=                              =");
  Serial.println("================================");
}

void Display_refreshMenu(char* title, char menuItems[][MENU_ITEM_LENGTH], uint8_t count, uint8_t currentMenuIndex) {
  for (uint8_t i = 0; i < 10; i++) {
    Serial.println();
  }
  Serial.println("================================");
  
  Serial.print("="); Serial.print(title);
  for (uint8_t i = 0; i < SCREEN_WIDTH - strlen(title); i++) {
    Serial.print(" ");
  }
  Serial.println("=");
  
  Serial.println("=------------------------------=");
  
  for (uint8_t i = 0; i < count; i++) {
    Serial.print("=");
    if (i == currentMenuIndex) {
      Serial.print("*");
    } else {
      Serial.print(" ");
    }
    Serial.print(menuItems[i]);
    for (uint8_t j = 0; j < SCREEN_WIDTH - strlen(menuItems[i]) - 1; j++) {
      Serial.print(" ");
    }
    Serial.println("=");
  }

  for (uint8_t i = 0; i < SCREEN_LINES - count - 2; i++) {
    Serial.println("=                              =");
  }
  
  Serial.println("================================");
}

#endif
