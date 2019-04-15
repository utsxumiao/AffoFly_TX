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
#if defined(CHECK_FREE_MEMORY)
  #include "MemoryFree.h"
#endif

#define MENU_STR_TOP_TITLE_IDX          0
#define MENU_STR_TOP_0_IDX              5
#define MENU_STR_TOP_1_IDX              13
#define MENU_STR_TOP_2_IDX              23
#define MENU_STR_TOP_3_IDX              31
#define MENU_STR_TOP_4_IDX              39
#define MENU_STR_TOP_LENGTH             47

#define MENU_STR_SETTING_TITLE_IDX      0
#define MENU_STR_SETTING_0_IDX          8
#define MENU_STR_SETTING_1_IDX          19
#define MENU_STR_SETTING_2_IDX          30
#define MENU_STR_SETTING_LENGTH         38

#define MENU_STR_RX_TITLE_IDX           0
#define MENU_STR_RX_0_IDX               11
#define MENU_STR_RX_LENGTH              ((RX_TOTAL_COUNT * (RX_NAME_MAX_LENGTH + 1)) + 11)

#define MENU_STR_RX_SETTING_0_IDX       0
#define MENU_STR_RX_SETTING_1_IDX       7
#define MENU_STR_RX_SETTING_2_IDX       14
#define MENU_STR_RX_SETTING_CHAN_IDX    19
#define MENU_STR_RX_SETTING_LENGTH      32

#define MENU_CHANNEL_DISPLAY_LENGTH     12

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

// RX setting menu items
const char menu_rx_setting_0[] PROGMEM = "Select";
const char menu_rx_setting_1[] PROGMEM = "Rename";
const char menu_rx_setting_2[] PROGMEM = "Bind";

MenuNode* topMenu = NULL;
MenuNode* currentMenu = NULL;
MenuNode* rxSettingTemplate = NULL;
MenuNode* rxRenameTemplate = NULL;
MenuItemEdit itemEdit;
MenuPagination pagination;
RxConfig selectedRxConfig;

char *menu_top_string = NULL;
char *menu_setting_string = NULL;
char *menu_rx_string = NULL;
char *menu_rx_setting_string = NULL;


void Menu_init() {
#if defined(CHECK_FREE_MEMORY)
  Serial.print("Menu_init() before. Free Memory: ");
  Serial.println(freeMemory());
#endif

  Menu_initButtons();
  setupMenu();
  showMenu(currentMenu);

#if defined(CHECK_FREE_MEMORY)
  Serial.print("Menu_init() after. Free Memory: ");
  Serial.println(freeMemory());
#endif
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
#ifdef BUZZER
      BuzzerBeepPattern buzzerPattern = buttonPress;
      Buzzer_start(buzzerPattern);
#endif
    }
  }
}

void setupMenu() {
  MenuNode* settingMenu;
  MenuNode* rxMenu;

  if (topMenu)  {
    return; // do not run the rest of the code if 'topMenu' has been allocated, which means that the menu structure is already set up.
  }

  // allocate strings first
  if (!menu_top_string)  {
    menu_top_string = (char*) malloc (MENU_STR_TOP_LENGTH);
    memset(menu_top_string, 0, MENU_STR_TOP_LENGTH);
  }
  if (!menu_setting_string)  {
    menu_setting_string = (char*) malloc (MENU_STR_SETTING_LENGTH);
    memset(menu_setting_string, 0, MENU_STR_SETTING_LENGTH);
  }
  if (!menu_rx_string)  {
    menu_rx_string = (char*) malloc (MENU_STR_RX_LENGTH);
    memset(menu_rx_string, 0, MENU_STR_RX_LENGTH);
  }
  if (!menu_rx_setting_string)  {
    menu_rx_setting_string = (char*) malloc (MENU_STR_RX_SETTING_LENGTH);
    memset(menu_rx_setting_string, 0, MENU_STR_RX_SETTING_LENGTH);
  }

  // Top menu --------------------
  topMenu = (MenuNode*)malloc(sizeof(MenuNode));
  uint8_t topMenuNodeItemCount = 2;
#ifdef SIMULATOR
  topMenuNodeItemCount++;
#endif
#ifdef BUDDY
  topMenuNodeItemCount += 2;
#endif
  char* currentMenuStr;
  uint8_t itemLength = strlen(menu_top_title);
  currentMenuStr = &menu_top_string[MENU_STR_TOP_TITLE_IDX];
  getValueFromProgmem(menu_top_title, currentMenuStr, itemLength);
  initMenuNode(topMenu, currentMenuStr, topMenuNodeItemCount);

  uint8_t topMenuNodeItemIndex = 0;
  itemLength = strlen(menu_top_0);
  currentMenuStr = &menu_top_string[MENU_STR_TOP_0_IDX];
  getValueFromProgmem(menu_top_0, currentMenuStr, itemLength);
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_CONTROL, currentMenuStr);

#ifdef SIMULATOR
  topMenuNodeItemIndex++;
  itemLength = strlen(menu_top_1);
  currentMenuStr = &menu_top_string[MENU_STR_TOP_1_IDX];
  getValueFromProgmem(menu_top_1, currentMenuStr, itemLength);
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_SIMULATOR, currentMenuStr);
#endif
#ifdef BUDDY
  topMenuNodeItemIndex++;
  itemLength = strlen(menu_top_2);
  currentMenuStr = &menu_top_string[MENU_STR_TOP_2_IDX];
  getValueFromProgmem(menu_top_2, currentMenuStr, itemLength);
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_TRAINER, currentMenuStr);

  topMenuNodeItemIndex++;
  itemLength = strlen(menu_top_3);
  currentMenuStr = &menu_top_string[MENU_STR_TOP_3_IDX];
  getValueFromProgmem(menu_top_3, currentMenuStr, itemLength);
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_STUDENT, currentMenuStr);
#endif
  topMenuNodeItemIndex++;
  itemLength = strlen(menu_top_4);
  currentMenuStr = &menu_top_string[MENU_STR_TOP_4_IDX];
  getValueFromProgmem(menu_top_4, currentMenuStr, itemLength);
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_SETTING, currentMenuStr);

  currentMenu = topMenu;  // Top menu is seleted by default

  // Setting menu ---------------------
  settingMenu = (MenuNode*)malloc(sizeof(MenuNode));
  itemLength = strlen(menu_setting_title);
  currentMenuStr = &menu_setting_string[MENU_STR_SETTING_TITLE_IDX];
  getValueFromProgmem(menu_setting_title, currentMenuStr, itemLength);
  initMenuNode(settingMenu, currentMenuStr, 3);

  itemLength = strlen(menu_setting_0);
  currentMenuStr = &menu_setting_string[MENU_STR_SETTING_0_IDX];
  getValueFromProgmem(menu_setting_0, currentMenuStr, itemLength);
  initMenuNodeItem(settingMenu->Items, 0, MENU_ID_SETTING_RX, currentMenuStr);

  itemLength = strlen(menu_setting_1);
  currentMenuStr = &menu_setting_string[MENU_STR_SETTING_1_IDX];
  getValueFromProgmem(menu_setting_1, currentMenuStr, itemLength);
  initMenuNodeItem(settingMenu->Items, 1, MENU_ID_SETTING_DATA_RST, currentMenuStr);

  itemLength = strlen(menu_setting_2);
  currentMenuStr = &menu_setting_string[MENU_STR_SETTING_2_IDX];
  getValueFromProgmem(menu_setting_2, currentMenuStr, itemLength);
  initMenuNodeItem(settingMenu->Items, 2, MENU_ID_SETTING_SOFT_RST, currentMenuStr);

  settingMenu->Prev = topMenu;  // Link back to the previous menu node
  topMenu->Items[4].Item = settingMenu;  // Link with the menu item in the previous menu node

  // RX Selection menu ----------------
  rxMenu = (MenuNode*)malloc(sizeof(MenuNode));
  itemLength = strlen(menu_rx_title);
  currentMenuStr = &menu_rx_string[MENU_STR_RX_TITLE_IDX];
  getValueFromProgmem(menu_rx_title, currentMenuStr, itemLength);
  initMenuNode(rxMenu, currentMenuStr, RX_TOTAL_COUNT);

  initMenuNodeItem(rxMenu->Items, 0, MENU_ID_RX_0, getRxName(0, &menu_rx_string[MENU_STR_RX_0_IDX]));
  initMenuNodeItem(rxMenu->Items, 1, MENU_ID_RX_1, getRxName(1, &menu_rx_string[MENU_STR_RX_0_IDX + ((RX_NAME_MAX_LENGTH + 1) * 1)]));
  initMenuNodeItem(rxMenu->Items, 2, MENU_ID_RX_2, getRxName(2, &menu_rx_string[MENU_STR_RX_0_IDX + ((RX_NAME_MAX_LENGTH + 1) * 2)]));
  initMenuNodeItem(rxMenu->Items, 3, MENU_ID_RX_3, getRxName(3, &menu_rx_string[MENU_STR_RX_0_IDX + ((RX_NAME_MAX_LENGTH + 1) * 3)]));
  initMenuNodeItem(rxMenu->Items, 4, MENU_ID_RX_4, getRxName(4, &menu_rx_string[MENU_STR_RX_0_IDX + ((RX_NAME_MAX_LENGTH + 1) * 4)]));
  initMenuNodeItem(rxMenu->Items, 5, MENU_ID_RX_5, getRxName(5, &menu_rx_string[MENU_STR_RX_0_IDX + ((RX_NAME_MAX_LENGTH + 1) * 5)]));
  initMenuNodeItem(rxMenu->Items, 6, MENU_ID_RX_6, getRxName(6, &menu_rx_string[MENU_STR_RX_0_IDX + ((RX_NAME_MAX_LENGTH + 1) * 6)]));
  initMenuNodeItem(rxMenu->Items, 7, MENU_ID_RX_7, getRxName(7, &menu_rx_string[MENU_STR_RX_0_IDX + ((RX_NAME_MAX_LENGTH + 1) * 7)]));
  initMenuNodeItem(rxMenu->Items, 8, MENU_ID_RX_8, getRxName(8, &menu_rx_string[MENU_STR_RX_0_IDX + ((RX_NAME_MAX_LENGTH + 1) * 8)]));
  initMenuNodeItem(rxMenu->Items, 9, MENU_ID_RX_9, getRxName(9, &menu_rx_string[MENU_STR_RX_0_IDX + ((RX_NAME_MAX_LENGTH + 1) * 9)]));
  rxMenu->Prev = settingMenu;  // Link back to the previous menu node
  settingMenu->Items[0].Item = rxMenu;  // Link with the menu item in the previous menu node

  // RX Setting template --------------------
  rxSettingTemplate = (MenuNode*)malloc(sizeof(MenuNode));
  initMenuNode(rxSettingTemplate, "", 4);

  itemLength = strlen(menu_rx_setting_0);
  currentMenuStr = &menu_rx_setting_string[MENU_STR_RX_SETTING_0_IDX];
  getValueFromProgmem(menu_rx_setting_0, currentMenuStr, itemLength);
  initMenuNodeItem(rxSettingTemplate->Items, 0, MENU_ID_RX_SETTING_SELECT, currentMenuStr);

  itemLength = strlen(menu_rx_setting_1);
  currentMenuStr = &menu_rx_setting_string[MENU_STR_RX_SETTING_1_IDX];
  getValueFromProgmem(menu_rx_setting_1, currentMenuStr, itemLength);
  initMenuNodeItem(rxSettingTemplate->Items, 1, MENU_ID_RX_SETTING_RENAME, currentMenuStr);

  itemLength = strlen(menu_rx_setting_2);
  currentMenuStr = &menu_rx_setting_string[MENU_STR_RX_SETTING_2_IDX];
  getValueFromProgmem(menu_rx_setting_2, currentMenuStr, itemLength);
  initMenuNodeItem(rxSettingTemplate->Items, 2, MENU_ID_RX_SETTING_BIND, currentMenuStr);

  itemLength = MENU_CHANNEL_DISPLAY_LENGTH;
  currentMenuStr = &menu_rx_setting_string[MENU_STR_RX_SETTING_CHAN_IDX];
  memset(currentMenuStr, 0, (itemLength + 1));
  initMenuNodeItem(rxSettingTemplate->Items, 3, 0, currentMenuStr);

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

char* getRxName(uint8_t index, char* rxBuffer)  {
  RxConfig tempRxConfig;
  EEPROM_readRxConfig(index + 1, &tempRxConfig);
  strncpy(rxBuffer, tempRxConfig.Name, RX_NAME_MAX_LENGTH);
  rxBuffer[RX_NAME_MAX_LENGTH] = '\0';
  return rxBuffer;
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
  if (itemCount > 0)  {
    node->Items = (MenuNodeItem*)malloc(itemCount * sizeof(MenuNodeItem));
    memset(node->Items, 0, (itemCount * sizeof(MenuNodeItem)));
  }
  else  {
    node->Items = NULL;
  }
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
      if (selectedNodeItem) {
        temp = selectedNodeItem->Item;
        menuId = selectedNodeItem->Id;
        title = selectedNodeItem->Menu;
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

void getRxChannelStr(uint8_t channel)  {
  char *rxChannelStr = &menu_rx_setting_string[MENU_STR_RX_SETTING_CHAN_IDX];
  char strChannel[4];
  memset(rxChannelStr, 0, MENU_CHANNEL_DISPLAY_LENGTH + 1);
  itoa(channel, strChannel, 10);
  strcpy(rxChannelStr, "Channel: ");
  strcat(rxChannelStr, strChannel);
}

void handleMenu(uint8_t menuId, char* title) {
  if (menuId >= MENU_ID_RX_0 && menuId <= MENU_ID_RX_9) { // Specific RX has been selected
    EEPROM_readRxConfig(menuId - MENU_ID_RX_0 + 1, &selectedRxConfig);
    rxSettingTemplate->ParentId = menuId;
    rxSettingTemplate->ParentMenu = title;
    getRxChannelStr(selectedRxConfig.RadioChannel);
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
        EEPROM_readRxConfig(selectedRxConfig.Id, &CURRENT_RX_CONFIG);
        break;
      case MENU_ID_RX_SETTING_BIND:
        TX_MODE = MODE_BIND;
        break;
      case MENU_ID_RX_RENAME_OK:
        strcpy(rxRenameTemplate->ParentMenu, title);
        strncpy(selectedRxConfig.Name, title, RX_NAME_MAX_LENGTH);
        selectedRxConfig.Name[RX_NAME_MAX_LENGTH] = '\0';
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
  // clear the string first. Assume len + 1 for the string
  memset(value, 0, len + 1);
  
  for (uint8_t i = 0; i < len; i++) {
    value[i] = pgm_read_byte_near(item + i);
  }
}

void Menu_stop() {

#if defined(CHECK_FREE_MEMORY)
  Serial.print("Menu_stop() before. Free Memory: ");
  Serial.println(freeMemory());
#endif

  // free menuNodes
  clearMenuNode(topMenu);
  topMenu = NULL;
  
  if (rxSettingTemplate)  {
    free(rxSettingTemplate->Items);
    free(rxSettingTemplate);
    rxSettingTemplate = NULL;
  }

  if (rxRenameTemplate) {
    free(rxRenameTemplate);
    rxRenameTemplate = NULL;
  }

  if (menu_top_string)  {
    free(menu_top_string);
    menu_top_string = NULL;
  }
  if (menu_setting_string)  {
    free(menu_setting_string);
    menu_setting_string = NULL;
  }
  if (menu_rx_string)  {
    free(menu_rx_string);
    menu_rx_string = NULL;
  }
  if (menu_rx_setting_string)  {
    free(menu_rx_setting_string);
    menu_rx_setting_string = NULL;
  }

  if (itemEdit.Value) {
    free(itemEdit.Value);
    itemEdit.Value = NULL;
  }

#if defined(CHECK_FREE_MEMORY)
  Serial.print("Menu_stop() after. Free Memory: ");
  Serial.println(freeMemory());
#endif
}

void clearMenuNode(MenuNode* node)  {
  MenuNode* temp;
  uint8_t i, count, itemIndex;

  while (node)  {
    i = 0;
    itemIndex = 255;
    count = node->ItemCount;
    while (i < count) {
      if (node->Items[i].Item)  {
        node = node->Items[i].Item;
        count = node->ItemCount;
        itemIndex = i;
        i = 0;
      }
      else  {
        i++;
      }
    }

    free(node->Items);
    node->Items = NULL;
    node->ItemCount = 0;

    if (node->Prev) {
      temp = node->Prev;
      if (itemIndex != 255) {
        free(temp->Items[itemIndex].Item);
        temp->Items[itemIndex].Item = NULL;
      }
      node = temp;
    }
    else  {
      free(node);
      node = NULL;
    }
  }
}
