#include "Arduino.h"
#include <U8g2lib.h>
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "Battery.h"
#include "AffoFly_Transmitter.h"
#include "Screen.h"

#ifdef SCREEN_LCD_SSD1306_128X64_HW_I2C

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

static const uint8_t pixelsPerTrim = 1;
static const uint8_t rcValuePerPixel = 25;
static const uint32_t controlScreenRefreshInterval = 2000000; // actively refresh control screen every 5 seconds
uint32_t previousControlScreenRefreshTime = 0;

void Screen_init() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_profont12_tr);
}

void Screen_showWelcomeScreen(char* projectName, char* projectVersion) {
  u8g2.firstPage();
  do {
    u8g2.drawStr(30, 30, projectName);
    u8g2.drawStr(40, 50, projectVersion);
  } while (u8g2.nextPage());
}

void Screen_showModeScreen(uint8_t txMode) {
  u8g2.firstPage();
  do {
    char text[10];
    switch (txMode) {
      case MODE_CONTROL:
        u8g2.setCursor(45, 30);
        u8g2.print(F("CONTROL"));
        break;
      case MODE_SIMULATOR:
        u8g2.setCursor(40, 30);
        u8g2.print(F("SIMULATOR"));
        break;
      case MODE_TRAINER:
        u8g2.setCursor(45, 30);
        u8g2.print(F("TRAINER"));
        break;
      case MODE_STUDENT:
        u8g2.setCursor(45, 30);
        u8g2.print(F("STUDENT"));
        break;
      case MODE_BIND:
        u8g2.setCursor(50, 30);
        u8g2.print(F("BIND"));
        break;
      case MODE_MENU:
        u8g2.setCursor(50, 30);
        u8g2.print(F("MENU"));
        break;
      default:
        break;
    }
  } while (u8g2.nextPage());
}

void Screen_showMenu(char* title, MenuNodeItem* items, uint8_t count, uint8_t index, void (*displayFunc)(uint8_t*)) {
  // Title
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 10);
    u8g2.print(title);
    u8g2.drawLine(0, 15, 128, 15);

    // Items
    if (displayFunc) {
      uint8_t lineCount = 0;
      displayFunc(&lineCount);
    } else {
      uint8_t menuItemStartX = 10;
      uint8_t menuItemStartY = 27;
      uint8_t lineGap = 12;
      uint8_t j = 0;
      for (uint8_t i = pagination.StartIndex; i <= pagination.EndIndex; i++) {
        if (i == index) {
          u8g2.setCursor(0, menuItemStartY + lineGap * j);
          u8g2.print(F("*"));
        }
        if (count > j) {
          u8g2.setCursor(menuItemStartX, menuItemStartY + lineGap * j);
          u8g2.print(items[i].Menu);
        }
        j++;
      }
    }
  } while (u8g2.nextPage());
}

void Screen_showMenuRxRename(uint8_t *lineCount) {
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 10);
    u8g2.print(F("RX RENAMING"));
    u8g2.drawLine(0, 15, 128, 15);

    for (uint8_t i = 0; i < strlen(itemEdit.Value); i++) {
      u8g2.setCursor(6 * i + 10, 27);
      if (i == itemEdit.Index) {
        u8g2.setFontMode(1);
        u8g2.drawBox(6 * i + 10 - 1, 18, 7, 10);
        u8g2.setDrawColor(2);
        u8g2.print(itemEdit.Value[i]);
        u8g2.setFontMode(0);
        u8g2.setDrawColor(1);
      } else {
        u8g2.print(itemEdit.Value[i]);
      }
    }
  } while (u8g2.nextPage());
}

void Screen_showControlScreen(ControlData controlData, RateData rateData, bool trimming, uint8_t trimStickIndex, uint32_t currentTime, bool forceExecute) {
  if (forceExecute || currentTime - previousControlScreenRefreshTime >= controlScreenRefreshInterval) {
    previousControlScreenRefreshTime = currentTime;
    u8g2.firstPage();
    do {
      // Voltage
      if (LOW_VOLTAGE) {
        u8g2.setFontMode(1);
        u8g2.drawBox(0, 0, 25, 12);
        u8g2.setDrawColor(2);
      }
      u8g2.setCursor(1, 10);
      u8g2.print(BATTERY_VOLTAGE);
      u8g2.setCursor(19, 10);
      u8g2.print("v");
      if (LOW_VOLTAGE) {
        u8g2.setFontMode(0);
        u8g2.setDrawColor(1);
      }

      // RX
      u8g2.setCursor(40, 10);
      u8g2.print(CURRENT_RX_CONFIG.Name);

      // Rate
      if (TX_MODE == MODE_CONTROL || TX_MODE == MODE_STUDENT) {
        u8g2.setCursor(110, 10);
        u8g2.print(rateData.RadioRate);
      }
      if (TX_MODE == MODE_SIMULATOR || TX_MODE == MODE_TRAINER) {
        u8g2.setCursor(100, 10);
        u8g2.print(rateData.CPPMRate);
      }
      u8g2.drawLine(0, 15, 128, 15);

      // Trimming
      // YAW
      u8g2.drawLine(5, 61, 49, 61);
      u8g2.drawLine(27, 59, 27, 63);
      u8g2.drawBox(26 + CURRENT_RX_CONFIG.YawTrim * pixelsPerTrim, 60, 3, 3);
      // ROL
      u8g2.drawLine(78, 61, 122, 61);
      u8g2.drawLine(100, 59, 100, 63);
      u8g2.drawBox(99 + CURRENT_RX_CONFIG.RollTrim * pixelsPerTrim, 60, 3, 3);
      // THR
      u8g2.drawLine(58, 16, 58, 60);
      u8g2.drawLine(56, 38, 60, 38);
      u8g2.drawBox(57, 37 - CURRENT_RX_CONFIG.ThrottleTrim * pixelsPerTrim, 3, 3);
      // PIT
      u8g2.drawLine(69, 16, 69, 60);
      u8g2.drawLine(67, 38, 71, 38);
      u8g2.drawBox(68, 37 - CURRENT_RX_CONFIG.PitchTrim * pixelsPerTrim, 3, 3);
      // Current Stick Pointer
      if (trimming) {
        switch (trimStickIndex) {
          case 0: //YAW
            u8g2.drawLine(52, 60, 52, 62);
            break;
          case 1: //THR
            u8g2.drawLine(57, 63, 59, 63);
            break;
          case 2: //PIT
            u8g2.drawLine(68, 63, 70, 63);
            break;
          case 3: //ROL
            u8g2.drawLine(75, 60, 75, 62);
            break;
        }
      }

      // AUX & SWD Channels
      uint8_t channelY = 27;
#ifdef EXTEND_CHANNEL
      uint8_t channelYGap = 10;
#else
      uint8_t channelYGap = 13;
#endif
      u8g2.setCursor(0, channelY);
      u8g2.print(F("A1"));
      u8g2.drawFrame(13, channelY - 8, 40, 8);
      u8g2.drawBox(13, channelY - 8, (controlData.Aux1 - 1000) / rcValuePerPixel, 8);

      u8g2.setCursor(117, channelY);
      u8g2.print(F("A2"));
      u8g2.drawFrame(75, channelY - 8, 40, 8);
      u8g2.drawBox(75, channelY - 8, (controlData.Aux2 - 1000) / rcValuePerPixel, 8);

      channelY += channelYGap;
      u8g2.setCursor(0, channelY);
      u8g2.print(F("A3"));
      u8g2.drawFrame(13, channelY - 8, 40, 8);
      u8g2.drawBox(13, channelY - 8, (controlData.Aux3 - 1000) / rcValuePerPixel, 8);

      u8g2.setCursor(117, channelY);
      u8g2.print(F("A4"));
      u8g2.drawFrame(75, channelY - 8, 40, 8);
      u8g2.drawBox(75, channelY - 8, (controlData.Aux4 - 1000) / rcValuePerPixel, 8);

#ifdef EXTEND_CHANNEL
      channelY += channelYGap;
      u8g2.setCursor(0, channelY);
      u8g2.print(F("A5"));
      u8g2.drawFrame(13, channelY - 8, 40, 8);
      u8g2.drawBox(13, channelY - 8, (controlData.Aux5 - 1000) / rcValuePerPixel, 8);

      u8g2.setCursor(117, channelY);
      u8g2.print(F("A6"));
      u8g2.drawFrame(75, channelY - 8, 40, 8);
      u8g2.drawBox(75, channelY - 8, (controlData.Aux6 - 1000) / rcValuePerPixel, 8);
#endif

      channelY += channelYGap;
#ifdef SWD_1
      u8g2.setCursor(0, channelY);
      u8g2.print(F("S1"));
      u8g2.drawFrame(13, channelY - 8, 40, 8);
      u8g2.drawBox(13, channelY - 8, (controlData.Swd1 - 1000) / rcValuePerPixel, 8);
#else
#ifdef SHOW_RATE
      u8g2.setCursor(0, channelY);
      u8g2.print(F("RC: "));
      u8g2.setCursor(20, channelY);
      u8g2.print(rateData.RadioRate);
#endif
#endif
#ifdef SWD_2
      u8g2.setCursor(117, channelY);
      u8g2.print(F("S2"));
      u8g2.drawFrame(75, channelY - 8, 40, 8);
      u8g2.drawBox(75, channelY - 8, (controlData.Swd2 - 1000) / rcValuePerPixel, 8);
#else
#ifdef SHOW_RATE
      u8g2.setCursor(75, channelY);
      u8g2.print(F("CY: "));
      u8g2.setCursor(95, channelY);
      u8g2.print(rateData.LoopRate);
#endif
#endif
    } while (u8g2.nextPage());
  }
}

void Screen_showRadioBindingScreen(uint8_t channel, bool tokenIsReady, bool bound) {
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 10);
    u8g2.print(F("RX BIND"));
    u8g2.drawLine(0, 15, 128, 15);

    u8g2.setCursor(0, 27);
    if (channel != 0) {
      u8g2.print(F("Channel: "));
      u8g2.setCursor(6 * 9 - 1, 27);
      u8g2.print(channel);
    } else {
      u8g2.print(F("Analyzing channel..."));
    }


    if (tokenIsReady) {
      u8g2.setCursor(0, 39);
      u8g2.print(F("Token is ready."));
      u8g2.setCursor(0, 51);
      if (bound) {
        u8g2.print(F("RX Bound!"));
      } else {
        u8g2.print(F("Waiting for RX..."));
      }
    }

  } while (u8g2.nextPage());
}

#endif
