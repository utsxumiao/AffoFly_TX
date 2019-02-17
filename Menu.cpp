#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "MENU.h"
#include "Display.h"

#ifdef BUDDY
#define ROOT_MENU_ITEM_COUNT  5
#else
#define ROOT_MENU_ITEM_COUNT  3
#endif

char title[MENU_TITLE_LENGTH] = "TX MODE";
char rootItems[ROOT_MENU_ITEM_COUNT][MENU_ITEM_LENGTH] = {
  "CONTROL",
  "SIMULATOR",
#ifdef BUDDY
  "TRAINER",
  "STUDENT",
#endif
  "SETTING"
};

uint8_t currentMenuIndex = 0;

void Menu_init() {
  Menu_initButtons();
  showScreen();
}

void Menu_initButtons() {
  Bounce backBounce = Bounce();
  backBounce.attach(BUTTON_BACK_PIN, INPUT_PULLUP);
  backBounce.interval(5);
  bounces[1] = backBounce;
  Bounce goBounce = Bounce();
  goBounce.attach(BUTTON_GO_PIN, INPUT_PULLUP);
  goBounce.interval(5);
  bounces[1] = goBounce;
  Bounce prevBounce = Bounce();
  prevBounce.attach(BUTTON_PREV_PIN, INPUT_PULLUP);
  prevBounce.interval(5);
  bounces[2] = prevBounce;
  Bounce nextBounce = Bounce();
  nextBounce.attach(BUTTON_NEXT_PIN, INPUT_PULLUP);
  nextBounce.interval(5);
  bounces[3] = nextBounce;
}

void Menu_checkButtons() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    bounces[i].update();
    if (bounces[i].fell()) {
      switch (i) {
        case 0:
          back();
          break;
        case 1:
          go();
          break;
        case 2:
          prev();
          break;
        case 3:
          next();
          break;
        default:
          break;
      }
    }
  }
}

void back() {
  //TODO:
}

void go() {
  TX_MODE = currentMenuIndex;
}

void prev() {
  if (currentMenuIndex <= 0) {
    currentMenuIndex = ROOT_MENU_ITEM_COUNT;
  }
  currentMenuIndex--;
  showScreen();
}

void next() {
  if (currentMenuIndex >= ROOT_MENU_ITEM_COUNT - 1) {
    currentMenuIndex = 0;
  } else {
    currentMenuIndex++;
  }
  showScreen();
}

void showScreen() {
  Display_refreshMenu(title, rootItems, ROOT_MENU_ITEM_COUNT, currentMenuIndex);
}
