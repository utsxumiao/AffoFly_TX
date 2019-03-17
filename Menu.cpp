#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "EEPROM.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "Buzzer.h"
#include "Menu.h"
#include "Screen.h"

// Top menu items
const char menu_top_title[] PROGMEM = "MODE";
const char menu_top_0[] PROGMEM = "CONTROL";
const char menu_top_1[] PROGMEM = "SIMULATOR";
const char menu_top_2[] PROGMEM = "TRAINER";
const char menu_top_3[] PROGMEM = "STUDENT";
const char menu_top_4[] PROGMEM = "SETTING";

// Setting menu items
const char menu_setting_title[] PROGMEM = "SETTING";
const char menu_setting_0[] PROGMEM = "RX Setting";
const char menu_setting_1[] PROGMEM = "Data Reset";
const char menu_setting_2[] PROGMEM = "Restart";

// RX meu items
const char menu_rx_title[] PROGMEM = "RX SETTING";
const char allowed_chars[] PROGMEM = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -_:*";
char menu_rx[RX_TOTAL_COUNT][RX_NAME_MAX_LENGTH + 1];

// RX setting menu items
const char menu_rx_setting_0[] PROGMEM = "Select";
const char menu_rx_setting_1[] PROGMEM = "Rename";
const char menu_rx_setting_2[] PROGMEM = "Bind";

MenuNode* topMenu;
MenuNode* currentMenu;
MenuNode* rxSettingTemplate;
MenuNode* rxRenameTemplate;
MenuItemEdit itemEdit;
MenuPagination pagination;
RxConfig selectedRxConfig;

void Menu_init() {
  Menu_initButtons();
  setupMenu();
  showMenu(currentMenu);
}

void Menu_initButtons() {
  Bounce backBounce = Bounce();
  backBounce.attach(BUTTON_BACK_PIN, INPUT_PULLUP);
  backBounce.interval(BUTTON_DEBOUNCE_MS);
  bounces[0] = backBounce;
  Bounce goBounce = Bounce();
  goBounce.attach(BUTTON_GO_PIN, INPUT_PULLUP);
  goBounce.interval(BUTTON_DEBOUNCE_MS);
  bounces[1] = goBounce;
  Bounce prevBounce = Bounce();
  prevBounce.attach(BUTTON_PREV_PIN, INPUT_PULLUP);
  prevBounce.interval(BUTTON_DEBOUNCE_MS);
  bounces[2] = prevBounce;
  Bounce nextBounce = Bounce();
  nextBounce.attach(BUTTON_NEXT_PIN, INPUT_PULLUP);
  nextBounce.interval(BUTTON_DEBOUNCE_MS);
  bounces[3] = nextBounce;
}

void Menu_checkButtons() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    bounces[i].update();
    if (bounces[i].fell()) {
      switch (i) {
        case 0:
          goBackMenu();
          break;
        case 1:
          selectMenu();
          break;
        case 2:
          navigateMenu(currentMenu, -1);
          break;
        case 3:
          navigateMenu(currentMenu, 1);
          break;
        default:
          break;
      }

      BuzzerBeepPattern buzzerPattern = buttonPress;
      Buzzer_start(buzzerPattern);
    }
  }
}

void setupMenu() {
  MenuNode* settingMenu;
  MenuNode* rxMenu;

  // Top menu --------------------
  topMenu = (MenuNode*)malloc(sizeof(MenuNode));
  uint8_t topMenuNodeItemCount = 2;
#ifdef SIMULATOR
  topMenuNodeItemCount++;
#endif
#ifdef BUDDY
  topMenuNodeItemCount += 2;
#endif
  uint8_t itemLength = strlen(menu_top_title);
  char menuTopTitle[itemLength + 1];
  memset(menuTopTitle, 0, itemLength + 1);
  getValueFromProgmem(menu_top_title, menuTopTitle, itemLength);
  initMenuNode(topMenu, menuTopTitle, topMenuNodeItemCount);

  uint8_t topMenuNodeItemIndex = 0;
  itemLength = strlen(menu_top_0);
  char menuTop0[itemLength + 1];
  memset(menuTop0, 0, itemLength + 1);
  getValueFromProgmem(menu_top_0, menuTop0, itemLength);
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_CONTROL, menuTop0);

#ifdef SIMULATOR
  topMenuNodeItemIndex++;
  itemLength = strlen(menu_top_1);
  char menuTop1[itemLength + 1];
  memset(menuTop1, 0, itemLength + 1);
  getValueFromProgmem(menu_top_1, menuTop1, itemLength);
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_SIMULATOR, menuTop1);
#endif
#ifdef BUDDY
  topMenuNodeItemIndex++;
  itemLength = strlen(menu_top_2);
  char menuTop2[itemLength + 1];
  memset(menuTop2, 0, itemLength + 1);
  getValueFromProgmem(menu_top_2, menuTop2, itemLength);
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_TRAINER, menuTop2);

  topMenuNodeItemIndex++;
  itemLength = strlen(menu_top_3);
  char menuTop3[itemLength + 1];
  memset(menuTop3, 0, itemLength + 1);
  getValueFromProgmem(menu_top_3, menuTop3, itemLength);
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_STUDENT, menuTop3);
#endif
  topMenuNodeItemIndex++;
  itemLength = strlen(menu_top_4);
  char menuTop4[itemLength + 1];
  memset(menuTop4, 0, itemLength + 1);
  getValueFromProgmem(menu_top_4, menuTop4, itemLength);
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_SETTING, menuTop4);

  currentMenu = topMenu;  // Top menu is seleted by default

  // Setting menu ---------------------
  settingMenu = (MenuNode*)malloc(sizeof(MenuNode));
  itemLength = strlen(menu_setting_title);
  char menuSettingTitle[itemLength + 1];
  memset(menuSettingTitle, 0, itemLength + 1);
  getValueFromProgmem(menu_setting_title, menuSettingTitle, itemLength);
  initMenuNode(settingMenu, menuSettingTitle, 3);

  itemLength = strlen(menu_setting_0);
  char menuSetting0[itemLength + 1];
  memset(menuSetting0, 0, itemLength + 1);
  getValueFromProgmem(menu_setting_0, menuSetting0, itemLength);
  initMenuNodeItem(settingMenu->Items, 0, MENU_ID_SETTING_RX, menuSetting0);

  itemLength = strlen(menu_setting_1);
  char menuSetting1[itemLength + 1];
  memset(menuSetting1, 0, itemLength + 1);
  getValueFromProgmem(menu_setting_1, menuSetting1, itemLength);
  initMenuNodeItem(settingMenu->Items, 1, MENU_ID_SETTING_DATA_RST, menuSetting1);

  itemLength = strlen(menu_setting_2);
  char menuSetting2[itemLength + 1];
  memset(menuSetting2, 0, itemLength + 1);
  getValueFromProgmem(menu_setting_2, menuSetting2, itemLength);
  initMenuNodeItem(settingMenu->Items, 2, MENU_ID_SETTING_SOFT_RST, menuSetting2);

  settingMenu->Prev = topMenu;  // Link back to the previous menu node
  topMenu->Items[4].Item = settingMenu;  // Link with the menu item in the previous menu node

  // RX Selection menu ----------------
  rxMenu = (MenuNode*)malloc(sizeof(MenuNode));
  itemLength = strlen(menu_rx_title);
  char menuRxTitle[itemLength + 1];
  memset(menuRxTitle, 0, itemLength + 1);
  getValueFromProgmem(menu_rx_title, menuRxTitle, itemLength);
  initMenuNode(rxMenu, menuRxTitle, RX_TOTAL_COUNT);

  initMenuNodeItem(rxMenu->Items, 0, MENU_ID_RX_0, getRxName(0));
  initMenuNodeItem(rxMenu->Items, 1, MENU_ID_RX_1, getRxName(1));
  initMenuNodeItem(rxMenu->Items, 2, MENU_ID_RX_2, getRxName(2));
  initMenuNodeItem(rxMenu->Items, 3, MENU_ID_RX_3, getRxName(3));
  initMenuNodeItem(rxMenu->Items, 4, MENU_ID_RX_4, getRxName(4));
  initMenuNodeItem(rxMenu->Items, 5, MENU_ID_RX_5, getRxName(5));
  initMenuNodeItem(rxMenu->Items, 6, MENU_ID_RX_6, getRxName(6));
  initMenuNodeItem(rxMenu->Items, 7, MENU_ID_RX_7, getRxName(7));
  initMenuNodeItem(rxMenu->Items, 8, MENU_ID_RX_8, getRxName(8));
  initMenuNodeItem(rxMenu->Items, 9, MENU_ID_RX_9, getRxName(9));
  rxMenu->Prev = settingMenu;  // Link back to the previous menu node
  settingMenu->Items[1].Item = rxMenu;  // Link with the menu item in the previous menu node

  // RX Setting template --------------------
  rxSettingTemplate = (MenuNode*)malloc(sizeof(MenuNode));
  initMenuNode(rxSettingTemplate, "", 4);

  itemLength = strlen(menu_rx_setting_0);
  char menuRxSetting0[itemLength + 1];
  memset(menuRxSetting0, 0, itemLength + 1);
  getValueFromProgmem(menu_rx_setting_0, menuRxSetting0, itemLength);
  initMenuNodeItem(rxSettingTemplate->Items, 0, MENU_ID_RX_SETTING_SELECT, menuRxSetting0);

  itemLength = strlen(menu_rx_setting_1);
  char menuRxSetting1[itemLength + 1];
  memset(menuRxSetting1, 0, itemLength + 1);
  getValueFromProgmem(menu_rx_setting_1, menuRxSetting1, itemLength);
  initMenuNodeItem(rxSettingTemplate->Items, 1, MENU_ID_RX_SETTING_RENAME, menuRxSetting1);

  itemLength = strlen(menu_rx_setting_2);
  char menuRxSetting2[itemLength + 1];
  memset(menuRxSetting2, 0, itemLength + 1);
  getValueFromProgmem(menu_rx_setting_2, menuRxSetting2, itemLength);
  initMenuNodeItem(rxSettingTemplate->Items, 2, MENU_ID_RX_SETTING_BIND, menuRxSetting2);

  initMenuNodeItem(rxSettingTemplate->Items, 3, 0, "");
  rxSettingTemplate->Prev = rxMenu;  // Link back to the previous menu node

  // RX Rename template ---------------------
  rxRenameTemplate = (MenuNode*)malloc(sizeof(MenuNode));
  initMenuNode(rxRenameTemplate, "", 0);
  rxRenameTemplate->ExecFunc = showRxRename;
  rxRenameTemplate->Prev = rxSettingTemplate;  // Link back to the previous menu node

  // initialize itemEdit
  itemEdit.Value = (char*)malloc(LINE_ITEM_MAX_LEN + 1);
  memset(itemEdit.Value, 0, LINE_ITEM_MAX_LEN + 1);
  itemEdit.Index = ITEM_EDIT_NOT_SELECTED;
  itemEdit.MaxLength = LINE_ITEM_MAX_LEN;

  // initialize pagination
  setPagination(0);
}

char* getRxName(uint8_t index)  {
  RxConfig tempRxConfig = EEPROM_readRxConfig(index + 1);
  strncpy(menu_rx[index], tempRxConfig.Name, RX_NAME_MAX_LENGTH);
  menu_rx[index][RX_NAME_MAX_LENGTH] = '\0';
  return menu_rx[index];
}

void setPagination(uint8_t scrollIndex) {
  // initialize pagination
  pagination.StartIndex = scrollIndex;
  pagination.EndIndex = pagination.StartIndex + SCREEN_MENU_LINES - 1;
}

void initMenuNode(MenuNode* node, char* title, uint8_t itemCount) {
  node->Title = title;
  node->Index = 0;
  node->ScrollIndex = 0;
  node->ItemCount = itemCount;
  node->ParentId = 0;
  node->ParentMenu = NULL;
  node->Prev = NULL;
  node->ExecFunc = NULL;
  node->Items = (MenuNodeItem*)malloc(itemCount * sizeof(MenuNodeItem));
}

void initMenuNodeItem(MenuNodeItem* items, uint8_t index, uint8_t id, char* menu)  {
  MenuNodeItem* nodeItem = &items[index];
  nodeItem->Id = id;
  nodeItem->Menu = menu;
  nodeItem->Item = NULL;
  nodeItem->Selectable = true;
}

void initMenuNodeItemWithSelectable(MenuNodeItem* items, uint8_t index, uint8_t id, char* menu, bool selectable)  {
  MenuNodeItem* nodeItem = &items[index];
  initMenuNodeItem(items, index, id, menu);
  nodeItem->Selectable = selectable;
}

void navigateMenu(MenuNode* node, int8_t upOrDown) {
  bool itemEditMode = false;
  if (node) {
    if (itemEdit.Index == ITEM_EDIT_NOT_SELECTED) {
      if (upOrDown == 1)  {
        node->Index++;
        if (node->Index >= node->ItemCount) {
          node->Index = 0;
        }

        // work out the new ScrollIndex for the pagination
        if (node->Index == 0) {
          node->ScrollIndex = 0;
        }
        else if (node->Index >= (node->ScrollIndex - SCREEN_MENU_HEADER_ROWS) &&
                 node->Index < ((node->ScrollIndex - SCREEN_MENU_HEADER_ROWS) + SCREEN_MENU_LINES)) {
          // do nothing
        }
        else  {
          node->ScrollIndex++;
        }
      }
      else if (upOrDown == -1)  {
        if (node->Index == 0)  {
          node->Index = node->ItemCount - 1;
        }
        else  {
          node->Index--;
        }

        // work out the new ScrollIndex for the pagination
        if (node->Index == 0) {
          node->ScrollIndex = 0;
        }
        else if (node->Index >= (node->ScrollIndex - SCREEN_MENU_HEADER_ROWS) &&
                 node->Index < ((node->ScrollIndex - SCREEN_MENU_HEADER_ROWS) + SCREEN_MENU_LINES)) {
          // do nothing
        }
        else  {
          if (node->ScrollIndex == 0)  {
            node->ScrollIndex = node->ItemCount - SCREEN_MENU_LINES + SCREEN_MENU_HEADER_ROWS;
          }
          else  {
            node->ScrollIndex--;
          }
        }
      }
      setPagination(node->ScrollIndex);
      showMenu(node);
    }
    else  {
      itemEditMode = true;
      itemEdit.Value[itemEdit.Index] = getNextAllowedChar(itemEdit.Value[itemEdit.Index], upOrDown);
      showMenuItemEditMode(node);
    }
  }
}

uint8_t getNextAllowedChar(uint8_t ch, int8_t upOrDown) {
  uint8_t len = strlen(allowed_chars);
  char allowedChars[len + 1];
  memset(allowedChars, 0, (len + 1));
  getValueFromProgmem(allowed_chars, allowedChars, len);
  char *pch = strchr(allowedChars, ch);
  uint8_t index = 0;

  if (pch)  {
    index = (pch - allowedChars);

    if (upOrDown == 1) { // up
      if (index >= len - 1) {
        index = 0;
      }
      else  {
        index++;
      }
    }
    else  { // down
      if (index == 0)  {
        index = len - 1;
      }
      else  {
        index--;
      }
    }
  }
  return allowedChars[index];
}

MenuNodeItem* getCurrentMenuNodeItem(MenuNode* node)  {
  if (node) {
    for (int i = 0; i < node->ItemCount; i++) {
      if (node->Index == i && node->Items[i].Selectable) {
        return &(node->Items[i]);
      }
    }
  }
  return NULL;
}

void selectMenu() {
  if (currentMenu) {
    if (itemEdit.Index == ITEM_EDIT_NOT_SELECTED) {
      MenuNode* temp = NULL;
      MenuNodeItem* selectedNodeItem = getCurrentMenuNodeItem(currentMenu);
      uint8_t menuId = 0;
      char* title;
      Serial.print("CurrentMenu: ");  Serial.println(currentMenu->Title);
      if (selectedNodeItem) {
        temp = selectedNodeItem->Item;
        menuId = selectedNodeItem->Id;
        title = selectedNodeItem->Menu;
        Serial.print("SelectedNodeItem: "); Serial.println(selectedNodeItem->Menu);
      }

      if (temp) {
        currentMenu = temp;
        currentMenu->Index = 0;
        currentMenu->ScrollIndex = 0;

        // initialize pagination
        setPagination(currentMenu->ScrollIndex);

        showMenu(currentMenu);
      }
      else  {
        // initialize pagination
        setPagination(0);

        handleMenu(menuId, title);
      }
    }
    else  {
      itemEdit.Index++;
      if (itemEdit.Index >= itemEdit.MaxLength)  {
        itemEdit.Index = 0;
        // save the data
        if (currentMenu->ExecFunc)  {
          currentMenu->ExecFunc(true);
        }
      }
      else  {
        if (itemEdit.Value[itemEdit.Index] == '\0') {
          itemEdit.Value[itemEdit.Index] = ' ';   // add a space if null char is found.
        }
        showMenuItemEditMode(currentMenu);
      }
    }
  }
}

void handleMenu(uint8_t menuId, char* title) {
  if (menuId >= MENU_ID_RX_0 && menuId <= MENU_ID_RX_9) { // Specific RX has been selected
    selectedRxConfig = EEPROM_readRxConfig(menuId - MENU_ID_RX_0 + 1);
    rxSettingTemplate->ParentId = menuId;
    rxSettingTemplate->ParentMenu = title;
    char strChannel[4];
    initMenuNodeItem(rxSettingTemplate->Items, 3, 0, strcat("Channel: ", itoa(selectedRxConfig.RadioChannel, strChannel, 10)));
    currentMenu = rxSettingTemplate;
    currentMenu->Index = 0;  // resetting user's selection in the previous visit to the menu
    currentMenu->ScrollIndex = 0;
    showMenu(currentMenu);  // print menu
  }
  else if (menuId == MENU_ID_RX_SETTING_RENAME) {
    rxRenameTemplate->ParentId = MENU_ID_RX_SETTING_RENAME; // rxRenameTemplate->ParentId;
    rxRenameTemplate->ParentMenu = rxSettingTemplate->ParentMenu;
    currentMenu = rxRenameTemplate;
    currentMenu->Index = 0;  // resetting user's selection in the previous visit to the menu

    if (itemEdit.Index == ITEM_EDIT_NOT_SELECTED) {
      memset(itemEdit.Value, 0, LINE_ITEM_MAX_LEN + 1); // clear the string first
      strcpy(itemEdit.Value, rxRenameTemplate->ParentMenu);
      itemEdit.Index = 0;
      itemEdit.MaxLength = RX_NAME_MAX_LENGTH;
    }
    showMenuItemEditMode(currentMenu);
  }
  else  {
    switch (menuId) {
      case MENU_ID_TOP_CONTROL:
        TX_MODE = MODE_CONTROL;
        break;
#ifdef SIMULATOR
      case MENU_ID_TOP_SIMULATOR:
        TX_MODE = MODE_SIMULATOR;
        break;
#endif
#ifdef BUDDY
      case MENU_ID_TOP_TRAINER:
        TX_MODE = MODE_TRAINER;
        break;
      case MENU_ID_TOP_STUDENT:
        TX_MODE = MODE_STUDENT;
        break;
#endif
      case MENU_ID_RX_SETTING_SELECT:
        EEPROM_writeCurrentRxId(selectedRxConfig.Id);
        CURRENT_RX_CONFIG = EEPROM_readRxConfig(selectedRxConfig.Id);
        break;
      case MENU_ID_RX_SETTING_BIND:
        TX_MODE = MODE_BIND;
        break;
      case MENU_ID_RX_RENAME_OK:
        strcpy(rxRenameTemplate->ParentMenu, title);
        strncpy(selectedRxConfig.Name, title, RX_NAME_MAX_LENGTH);
        EEPROM_writeRxConfig(selectedRxConfig);
        break;
      case MENU_ID_SETTING_SOFT_RST:
        softReset();
        break;
      case MENU_ID_SETTING_DATA_RST:
        EEPROM_loadDefaults();
        softReset();
        break;
    }
  }
}

void goBackMenu() {
  if (currentMenu) {
    itemEdit.Index = ITEM_EDIT_NOT_SELECTED;
    if (currentMenu->Prev) {
      currentMenu = currentMenu->Prev;
    }
    setPagination(currentMenu->ScrollIndex);
    showMenu(currentMenu);  // print menu
  }
}

void showMenu(MenuNode* node) {
  showMenu(node, NULL);
}

void showMenu(MenuNode* node, void (*displayFunc)(uint8_t*))  {
  if (node) {
    char* title;
    if (node->ParentMenu) {
      title = node->ParentMenu;
    } else {
      title = node->Title;
    }
    Screen_showMenu(title, node->Items, node->ItemCount, node->Index, displayFunc);
  } else {
#ifdef DEBUG
    Serial.println(F("Menu not set!"));
#endif
  }
}

void showMenuItemEditMode(MenuNode* node) {
  switch (node->ParentId) {
    case MENU_ID_RX_SETTING_RENAME:
      showMenu(node, Screen_showMenuRxRename);  // print menu
      break;
  }
}

void showRxRename(bool saveMode) {
  if (saveMode) {
    handleMenu(MENU_ID_RX_RENAME_OK, itemEdit.Value);  // save the RX name from the user
    goBackMenu();
  }
}

void softReset() {
  asm volatile ("  jmp 0");
}

void getValueFromProgmem(char* item, char* value, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    value[i] = pgm_read_byte_near(item + i);
  }
}
