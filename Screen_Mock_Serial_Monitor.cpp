#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "Screen.h"

#ifdef SCREEN_SERIAL_MONITOR

const uint8_t SCREEN_WIDTH = 30;
const uint8_t SCREEN_LINES = 8;

void Display_showWelcomeScreen(char* projectName, char* projectVersion) {
  Serial.println(F("================================"));
  Serial.println(F("=                              ="));
  Serial.println(F("=                              ="));
  Serial.println(F("=                              ="));
  Serial.print(F("=          ")); Serial.print(projectName); Serial.println(F("          ="));
  Serial.print(F("=            ")); Serial.print(projectVersion); Serial.println(F("            ="));
  Serial.println(F("=                              ="));
  Serial.println(F("=                              ="));
  Serial.println(F("=                              ="));
  Serial.println(F("================================"));
}

void Display_showModeScreen(uint8_t txMode) {
  for (uint8_t i = 0; i < 10; i++) {
    Serial.println();
  }
  Serial.println(F("================================"));
  Serial.println(F("=                              ="));
  Serial.println(F("=                              ="));
  Serial.println(F("=                              ="));
  switch (txMode) {
    case MODE_CONTROL:
      Serial.println(F("=           CONTROL            ="));
      break;
    case MODE_SIMULATOR:
      Serial.println(F("=           SIMULATOR          ="));
      break;
    case MODE_TRAINER:
      Serial.println(F("=           TRAINER            ="));
      break;
    case MODE_STUDENT:
      Serial.println(F("=           STUDENT            ="));
      break;
    case MODE_BIND:
      Serial.println(F("=            BIND              ="));
      break;
    case MODE_MENU:
      Serial.println(F("=            MENU              ="));
      break;
    default:
      break;
  }
  Serial.println(F("=                              ="));
  Serial.println(F("=                              ="));
  Serial.println(F("=                              ="));
  Serial.println(F("=                              ="));
  Serial.println(F("================================"));
}

void Display_refreshMenu(char* title, MenuNodeItem* items, uint8_t count, uint8_t index) {
  for (uint8_t i = 0; i < 10; i++) {
    Serial.println();
  }
  Serial.println(F("================================"));
  
  Serial.print(F("= ")); Serial.print(title);
  for (uint8_t i = 0; i < SCREEN_WIDTH - strlen(title) - 1; i++) {
    Serial.print(F(" "));
  }
  Serial.println(F("="));
  
  Serial.println(F("=------------------------------="));
  
  for (uint8_t i = 0; i < count; i++) {
    Serial.print(F("="));
    if (i == index) {
      Serial.print(F("*"));
    } else {
      Serial.print(F(" "));
    }
    Serial.print(items[i].Menu);
    for (uint8_t j = 0; j < SCREEN_WIDTH - strlen(items[i].Menu) - 1; j++) {
      Serial.print(F(" "));
    }
    Serial.println(F("="));
  }

  for (uint8_t i = 0; i < SCREEN_LINES - count - 2; i++) {
    Serial.println(F("=                              ="));
  }
  
  Serial.println(F("================================"));
}

#endif