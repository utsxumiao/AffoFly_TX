#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "Battery.h"
#include "AffoFly_Transmitter.h"
#include "Screen.h"

#ifdef SCREEN_SERIAL_MONITOR

static const uint32_t controlScreenRefreshInterval = 200000;  //5hz
uint32_t previousControlScreenRefreshTime = 0;
bool controlScreenForceRefresh = false;

void Screen_init() {
}

void Screen_showWelcomeScreen(char* projectName, char* projectVersion) {
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

void Screen_showModeScreen(uint8_t txMode) {
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

void Screen_showMenu(char* title, MenuNodeItem* items, uint8_t count, uint8_t index, void (*displayFunc)(uint8_t*)) {
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

  if (displayFunc)  {
    uint8_t lineCount = 0;
    displayFunc(&lineCount);
  }
  else  {
    for (uint8_t i = 0; i < count; i++) {
      if (i >= pagination.StartIndex && i <= pagination.EndIndex) {
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
    }
  }
  for (uint8_t i = 0; i < SCREEN_LINES - count - 2; i++) {
    Serial.println(F("=                              ="));
  }

  Serial.println(F("================================"));
}

void Screen_showMenuRxRename(uint8_t *lineCount) {
  uint8_t i = 0;

  Serial.print("= ");
  Serial.print(itemEdit.Value);
  for (uint8_t j = 0; j < SCREEN_WIDTH - strlen(itemEdit.Value) - 1; j++) {
    Serial.print(F(" "));
  }
  Serial.println(F("="));

  Serial.print("= ");
  while (i < itemEdit.Index)  {
    Serial.print(" ");
    i++;
  }
  Serial.print("-");
  for (uint8_t j = 0; j < SCREEN_WIDTH - (i + 2); j++) {
    Serial.print(F(" "));
  }
  Serial.println(F("="));

  *lineCount = 2;
}

//void Screen_showControlScreen(ControlData controlData, RateData rateData, uint32_t currentTime, bool forceExecute) {
//  if (forceExecute || currentTime - previousControlScreenRefreshTime >= controlScreenRefreshInterval) {
//    previousControlScreenRefreshTime = currentTime;
//    Serial.print("RX Name: ");  Serial.print(CURRENT_RX_CONFIG.Name); Serial.print("    ");
//    Serial.print("TX Mode: ");  Serial.print(TX_MODE); Serial.print("    ");
//    Serial.print("Battery Voltage: ");  Serial.print(BATTERY_VOLTAGE); Serial.print("    ");
//    Serial.print("Low Voltage: ");  Serial.print(LOW_VOLTAGE); Serial.print("    ");
//    Serial.println();
//#ifdef SHOW_RATE
//    Serial.print("Loop Rate: ");  Serial.print(rateData.LoopRate); Serial.print("   ");
//    Serial.print("Button Check Rate: ");  Serial.print(rateData.ButtonCheckRate); Serial.print("   ");
//    Serial.print("Data Get Rate: ");  Serial.print(rateData.DataGetRate); Serial.print("   ");
//    Serial.print("Radio Rate: ");  Serial.print(rateData.RadioRate); Serial.print("   ");
//    Serial.print("CPPM Rate: ");  Serial.print(rateData.CPPMRate); Serial.print("   ");
//    Serial.println();
//#endif
//  }
//}

void Screen_showControlScreen(ControlData controlData, RateData rateData, bool trimming, uint8_t trimStickIndex, uint32_t currentTime, bool forceExecute) {
}

#endif
