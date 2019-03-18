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
  if (currentTime - previousControlScreenRefreshTime >= controlScreenRefreshInterval) {
    previousControlScreenRefreshTime = currentTime;
    for (uint8_t i = 0; i < 10; i++) {
      Serial.println();
    }
    Serial.println(F("================================"));

    Serial.print(F("= ")); Serial.print(BATTERY_VOLTAGE); Serial.print("v"); 
    if (LOW_VOLTAGE) {
      Serial.print("!   ");
    } else {
      Serial.print("    ");
    }
    Serial.print(CURRENT_RX_CONFIG.Name); Serial.print("    "); Serial.print(rateData.RadioRate);
    for (uint8_t i = 0; i < SCREEN_WIDTH - strlen(CURRENT_RX_CONFIG.Name) - 15 - 1; i++) {
      Serial.print(F(" "));
    }
    Serial.println(F("="));

    Serial.print(F("= "));  Serial.print(F("A1: "));  Serial.print(controlData.Aux1); Serial.print("      ");
    Serial.print(F("A2: "));  Serial.print(controlData.Aux2);
    for (uint8_t i = 0; i < SCREEN_WIDTH - 22 - 1; i++) {
      Serial.print(F(" "));
    }
    Serial.println(F("="));
    
    Serial.print(F("= "));  Serial.print(F("A3: "));  Serial.print(controlData.Aux3); Serial.print("      ");
    Serial.print(F("A4: "));  Serial.print(controlData.Aux4);
    for (uint8_t i = 0; i < SCREEN_WIDTH - 22 - 1; i++) {
      Serial.print(F(" "));
    }
    Serial.println(F("="));

#ifdef EXTEND_CHANNEL
    Serial.print(F("= "));  Serial.print(F("A5: "));  Serial.print(controlData.Aux5); Serial.print("      ");
    Serial.print(F("A6: "));  Serial.print(controlData.Aux6);
    for (uint8_t i = 0; i < SCREEN_WIDTH - 22 - 1; i++) {
      Serial.print(F(" "));
    }
    Serial.println(F("="));
#endif

    Serial.print(F("= "));
#ifdef SWD1
    Serial.print(F("S1: "));  Serial.print(controlData.Swd1); Serial.print("      ");
#else
#ifdef SHOW_RATE
    Serial.print(F("RAD: "));  Serial.print(rateData.RadioRate);  Serial.print("      ");
#else
    Serial.print(F("          "));
#endif
#endif

#ifdef SWD2
    Serial.print(F("S2: "));  Serial.print(controlData.Swd2);
#else
#ifdef SHOW_RATE
    Serial.print(F("CYL: "));  Serial.print(rateData.LoopRate);
#else
    Serial.print(F("          "));
#endif
#endif
 
    for (uint8_t i = 0; i < SCREEN_WIDTH - 23 - 1; i++) {
      Serial.print(F(" "));
    }
    Serial.println(F("="));

    Serial.print(F("= "));  
    if(trimming && trimStickIndex == 0) Serial.print(F(">"));
    Serial.print(F("Y: "));  Serial.print(CURRENT_RX_CONFIG.YawTrim); Serial.print("  ");
    if(trimming && trimStickIndex == 1) Serial.print(F(">"));
    Serial.print(F("T: "));  Serial.print(CURRENT_RX_CONFIG.ThrottleTrim); Serial.print("  ");
    if(trimming && trimStickIndex == 2) Serial.print(F(">"));
    Serial.print(F("P: "));  Serial.print(CURRENT_RX_CONFIG.PitchTrim); Serial.print("  ");
    if(trimming && trimStickIndex == 3) Serial.print(F(">"));
    Serial.print(F("R: "));  Serial.print(CURRENT_RX_CONFIG.RollTrim);
    for (uint8_t i = 0; i < SCREEN_WIDTH - 28 - 1; i++) {
      Serial.print(F(" "));
    }
    Serial.println(F("="));

    Serial.println(F("================================"));
  }
}

void Screen_showRadioBindingScreen(uint8_t channel, bool tokenIsReady, bool bound) {
  for (uint8_t i = 0; i < 10; i++) {
    Serial.println();
  }
  Serial.println(F("================================"));

  Serial.print(F("= ")); Serial.print(F("RX BIND"));
  for (uint8_t i = 0; i < SCREEN_WIDTH - 7 - 1; i++) {
    Serial.print(F(" "));
  }
  Serial.println(F("="));
  Serial.println(F("=------------------------------="));

  Serial.print(F("= Channel: "));
  if (channel) {
    Serial.print(channel);
  } else {
    Serial.print(F("..."));
  }
  for (uint8_t i = 0; i < SCREEN_WIDTH - 12 - 1; i++) {
    Serial.print(F(" "));
  }
  Serial.println("=");

  Serial.print("= ");
  if (tokenIsReady) {
    Serial.print(F("Token ready"));
    for (uint8_t i = 0; i < SCREEN_WIDTH - 11 - 1; i++) {
      Serial.print(F(" "));
    }
  } else {
    for (uint8_t i = 0; i < SCREEN_WIDTH - 1; i++) {
      Serial.print(F(" "));
    }
  }
  Serial.println("=");

  if (channel && tokenIsReady) {
    Serial.print("= ");
    if (bound) {
      Serial.println(F("RX Bound!"));
      for (uint8_t i = 0; i < SCREEN_WIDTH - 9 - 1; i++) {
        Serial.print(F(" "));
      }
    } else {
      Serial.print(F("Waiting..."));
      for (uint8_t i = 0; i < SCREEN_WIDTH - 10 - 1; i++) {
        Serial.print(F(" "));
      }
    }
    Serial.println("=");
  } else {
    Serial.println(F("=                              ="));
  }

  for (uint8_t i = 0; i < SCREEN_LINES - 3 - 2; i++) {
    Serial.println(F("=                              ="));
  }
  Serial.println(F("================================"));
}

#endif
