#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "EEPROM.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "Menu.h"
#include "Screen.h"

// Top menu items
const char menu_top_title[] = "MODE";   // Menu title
const char menu_top_0[] = "CONTROL";
const char menu_top_1[] = "SIMULATOR";
const char menu_top_2[] = "TRAINER";
const char menu_top_3[] = "STUDENT";
const char menu_top_4[] = "SETTING";

// Setting menu items
const char menu_setting_title[] = "SETTING";
const char menu_setting_0[] = "TX ID";
const char menu_setting_1[] = "RX Setting";
const char menu_setting_2[] = "Data Reset";
const char menu_setting_3[] = "Restart";

// RX meu items
const char menu_rx_title[] = "RX SETTING";
const char allowed_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -_:*";

// RX setting menu items
const char menu_rx_setting_0[] = "Select";
const char menu_rx_setting_1[] = "Rename";
const char menu_rx_setting_2[] = "Bind";

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
    }
  }
}

void setupMenu()  {
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
  initMenuNode(topMenu, menu_top_title, topMenuNodeItemCount);
  uint8_t topMenuNodeItemIndex = 0;
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_CONTROL, menu_top_0);
#ifdef SIMULATOR
  topMenuNodeItemIndex++;
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_SIMULATOR, menu_top_1);
#endif
#ifdef BUDDY
  topMenuNodeItemIndex++;
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_TRAINER, menu_top_2);
  topMenuNodeItemIndex++;
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_STUDENT, menu_top_3);
#endif
  topMenuNodeItemIndex++;
  initMenuNodeItem(topMenu->Items, topMenuNodeItemIndex, MENU_ID_TOP_SETTING, menu_top_4);

  currentMenu = topMenu;  // Top menu is seleted by default

  // Setting menu ---------------------
  settingMenu = (MenuNode*)malloc(sizeof(MenuNode));
  initMenuNode(settingMenu, menu_setting_title, 4);
  initMenuNodeItem(settingMenu->Items, 0, MENU_ID_SETTING_TX, menu_setting_0);
  initMenuNodeItem(settingMenu->Items, 1, MENU_ID_SETTING_RX, menu_setting_1);
  initMenuNodeItem(settingMenu->Items, 2, MENU_ID_SETTING_DATA_RST, menu_setting_2);
  initMenuNodeItem(settingMenu->Items, 3, MENU_ID_SETTING_SOFT_RST, menu_setting_3);
  settingMenu->Prev = topMenu;  // Link back to the previous menu node
  topMenu->Items[4].Item = settingMenu;  // Link with the menu item in the previous menu node

  // RX Selection menu ----------------
  rxMenu = (MenuNode*)malloc(sizeof(MenuNode));
  initMenuNode(rxMenu, menu_rx_title, 10);
  initMenuNodeItem(rxMenu->Items, 0, MENU_ID_RX_0, getAllocatedRxName(0));
  initMenuNodeItem(rxMenu->Items, 1, MENU_ID_RX_1, getAllocatedRxName(1));
  initMenuNodeItem(rxMenu->Items, 2, MENU_ID_RX_2, getAllocatedRxName(2));
  initMenuNodeItem(rxMenu->Items, 3, MENU_ID_RX_3, getAllocatedRxName(3));
  initMenuNodeItem(rxMenu->Items, 4, MENU_ID_RX_4, getAllocatedRxName(4));
  initMenuNodeItem(rxMenu->Items, 5, MENU_ID_RX_5, getAllocatedRxName(5));
  initMenuNodeItem(rxMenu->Items, 6, MENU_ID_RX_6, getAllocatedRxName(6));
  initMenuNodeItem(rxMenu->Items, 7, MENU_ID_RX_7, getAllocatedRxName(7));
  initMenuNodeItem(rxMenu->Items, 8, MENU_ID_RX_8, getAllocatedRxName(8));
  initMenuNodeItem(rxMenu->Items, 9, MENU_ID_RX_9, getAllocatedRxName(9));
  rxMenu->Prev = settingMenu;  // Link back to the previous menu node
  settingMenu->Items[1].Item = rxMenu;  // Link with the menu item in the previous menu node

  // RX Setting template --------------------
  rxSettingTemplate = (MenuNode*)malloc(sizeof(MenuNode));
  initMenuNode(rxSettingTemplate, "", 4);
  initMenuNodeItem(rxSettingTemplate->Items, 0, MENU_ID_RX_SETTING_SELECT, menu_rx_setting_0);
  initMenuNodeItem(rxSettingTemplate->Items, 1, MENU_ID_RX_SETTING_RENAME, menu_rx_setting_1);
  initMenuNodeItem(rxSettingTemplate->Items, 2, MENU_ID_RX_SETTING_BIND, menu_rx_setting_2);
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

  // initialize pagination
  setPagination(0);
}

char* getAllocatedRxName(uint8_t index)  {
  RxConfig tempRxConfig = EEPROM_readRxConfig(index + 1);
  char* rxName = (char*)malloc(LINE_ITEM_MAX_LEN + 1);
  strncpy(rxName, tempRxConfig.Name, LINE_ITEM_MAX_LEN);
  rxName[LINE_ITEM_MAX_LEN] = '\0';
  return rxName;
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
  char *pch = strchr(allowed_chars, ch);
  uint8_t index = 0;

  if (pch)  {
    index = (pch - allowed_chars);

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
  return allowed_chars[index];
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
      if (itemEdit.Index >= LINE_ITEM_MAX_LEN)  {
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
    currentMenu->Index = 0;  // resetting use's selection in the previous visit to the menu
    currentMenu->ScrollIndex = 0;
    showMenu(currentMenu);  // print menu
  }
  else if (menuId == MENU_ID_RX_SETTING_RENAME) {
    rxRenameTemplate->ParentId = MENU_ID_RX_SETTING_RENAME; // rxRenameTemplate->ParentId;
    rxRenameTemplate->ParentMenu = rxSettingTemplate->ParentMenu;
    currentMenu = rxRenameTemplate;
    currentMenu->Index = 0;  // resetting use's selection in the previous visit to the menu

    if (itemEdit.Index == ITEM_EDIT_NOT_SELECTED) {
      memset(itemEdit.Value, 0, LINE_ITEM_MAX_LEN + 1); // clear the string first
      strcpy(itemEdit.Value, rxRenameTemplate->ParentMenu);
      itemEdit.Index = 0;
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
      case MENU_ID_RX_SETTING_BIND:
        TX_MODE = MODE_BIND;
        break;
      case MENU_ID_RX_RENAME_OK:
        strcpy(rxRenameTemplate->ParentMenu, title);
        strncpy(selectedRxConfig.Name, title, LINE_ITEM_MAX_LEN);
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
    Display_refreshMenu(title, node->Items, node->ItemCount, node->Index, displayFunc);
  } else {
#ifdef DEBUG
    Serial.println(F("Menu not set!"));
#endif
  }
}

void showMenuItemEditMode(MenuNode* node) {
    switch (node->ParentId) {
      case MENU_ID_RX_SETTING_RENAME:
        showMenu(node, Display_showRxRename);  // print menu
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
