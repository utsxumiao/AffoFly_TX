#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "EEPROM.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "MENU.h"
#include "Display.h"

#define MENU_ID_TOP_CONTROL         10
#ifdef SIMULATOR
#define MENU_ID_TOP_SIMULATOR       11
#endif
#ifdef BUDDY
#define MENU_ID_TOP_TRAINER         12
#define MENU_ID_TOP_STUDENT         13
#endif
#define MENU_ID_TOP_SETTING         14

#define MENU_ID_SETTING_TX          20
#define MENU_ID_SETTING_RX          21
#define MENU_ID_SETTING_DATA_RST    22
#define MENU_ID_SETTING_SOFT_RST    23

#define MENU_ID_RX_0                30
#define MENU_ID_RX_1                31
#define MENU_ID_RX_2                32
#define MENU_ID_RX_3                33
#define MENU_ID_RX_4                34
#define MENU_ID_RX_5                35
#define MENU_ID_RX_6                36
#define MENU_ID_RX_7                37
#define MENU_ID_RX_8                38
#define MENU_ID_RX_9                39

#define MENU_ID_RX_SETTING_SELECT   40
#define MENU_ID_RX_SETTING_RENAME   41
#define MENU_ID_RX_SETTING_BIND     42

#define MENU_ID_RX_RENAME_OK        50

#define RX_NAME_MAX_LEN             13 //TODO: this should move to each screen and name it more generic like MAX-LINE-LENGTH

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
char menu_rx[][RX_NAME_MAX_LEN] = {"RX1 Name", "RX2 Name", "RX3 Name", "RX4 Name", "RX5 Name", "RX6 Name", "RX7 Name", "RX8 Name", "RX9 Name", "RX10 Name"};

// RX setting menu items
const char menu_rx_setting_0[] = "Select";
const char menu_rx_setting_1[] = "Rename";
const char menu_rx_setting_2[] = "Bind";


MenuNode* topMenu;
MenuNode* currentMenu;
MenuNode* rxSettingTemplate;
MenuNode* rxRenameTemplate;
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
  initMenuNodeItem(rxMenu->Items, 0, MENU_ID_RX_0, menu_rx[0]);
  initMenuNodeItem(rxMenu->Items, 1, MENU_ID_RX_1, menu_rx[1]);
  initMenuNodeItem(rxMenu->Items, 2, MENU_ID_RX_2, menu_rx[2]);
  initMenuNodeItem(rxMenu->Items, 3, MENU_ID_RX_3, menu_rx[3]);
  initMenuNodeItem(rxMenu->Items, 4, MENU_ID_RX_4, menu_rx[4]);
  initMenuNodeItem(rxMenu->Items, 5, MENU_ID_RX_5, menu_rx[5]);
  initMenuNodeItem(rxMenu->Items, 6, MENU_ID_RX_6, menu_rx[6]);
  initMenuNodeItem(rxMenu->Items, 7, MENU_ID_RX_7, menu_rx[7]);
  initMenuNodeItem(rxMenu->Items, 8, MENU_ID_RX_8, menu_rx[8]);
  initMenuNodeItem(rxMenu->Items, 9, MENU_ID_RX_9, menu_rx[9]);
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
  rxRenameTemplate->Prev = rxSettingTemplate;  // Link back to the previous menu node
}

void initMenuNode(MenuNode* node, char* title, uint8_t itemCount) {
  node->Title = title;
  node->Index = 0;
  node->ItemCount = itemCount;
  node->LookupItemCount = itemCount;
  node->ParentId = 0;
  node->ParentMenu = NULL;
  node->Prev = NULL;
  node->Items = (MenuNodeItem*)malloc(itemCount * sizeof(MenuNodeItem));
}

//void initMenuNodeWithParentData(MenuNode* node, char* title, uint8_t itemCount, char *parentMenu, uint8_t parentId) {
//  initMenuNode(node, title, itemCount);
//  node->ParentId = parentId;
//  node->ParentMenu = parentMenu;
//}

void initMenuNodeItem(MenuNodeItem* items, uint8_t index, uint8_t id, char* menu)  {
  MenuNodeItem* nodeItem = &items[index];
  nodeItem->Id = id;
  nodeItem->Menu = menu;
  nodeItem->Item = NULL;
}

void initMenuNodeItemWithLookup(MenuNodeItem* items, uint8_t index, uint8_t id, char* menu)  {
  MenuNodeItem* nodeItem = &items[index];
  initMenuNodeItem(items, index, id, menu);
}

void navigateMenu(MenuNode* node, int8_t upOrDown) {
  if (node) {
    if (upOrDown == 1)  {
      node->Index++;
      if (node->Index >= node->LookupItemCount) {
        node->Index = 0;
      }
    }
    else if (upOrDown == -1)  {
      if (node->Index == 0)  {
        node->Index = node->LookupItemCount - 1;
      }
      else  {
        node->Index--;
      }
    }
    showMenu(node);  // print menu
  }
}

void selectMenu() {
  if (currentMenu) {
    MenuNode* temp = NULL;

    bool lookupData = true;
    int j = 0;
    uint8_t menuId = 0;
    char* title;

    for (int i = 0; i < currentMenu->ItemCount; i++) {
      lookupData = true;

      if (lookupData) {
        if (currentMenu->Index == j)  {
          temp = currentMenu->Items[i].Item;
          menuId = currentMenu->Items[i].Id;
          title = currentMenu->Items[i].Menu;
        }
        j++;
      }
    }

    if (temp) {
      currentMenu = temp;
      currentMenu->Index = 0;  // resetting use's selection in the previous visit to the menu
      showMenu(currentMenu);  // print menu
    }
    else  { // No more sub-menu. Take action now
      //      Serial.println("No sub-menu found. Handling the menu"); // for testing only
      handleMenu(menuId, title); // Id distinguishes each menu
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
    showMenu(currentMenu);  // print menu
  }
  else if (menuId == MENU_ID_RX_SETTING_RENAME) {
    rxRenameTemplate->ParentId = rxRenameTemplate->ParentId;
    rxRenameTemplate->ParentMenu = rxSettingTemplate->ParentMenu;
    currentMenu = rxRenameTemplate;
    currentMenu->Index = 0;  // resetting use's selection in the previous visit to the menu

    showMenu(currentMenu);  // print menu
    showRxRename(rxRenameTemplate->ParentMenu);
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
      case MENU_ID_RX_0:
        
      case MENU_ID_RX_1:
      case MENU_ID_RX_2:
      case MENU_ID_RX_3:
      case MENU_ID_RX_4:
      case MENU_ID_RX_5:
      case MENU_ID_RX_6:
      case MENU_ID_RX_7:
      case MENU_ID_RX_8:
      case MENU_ID_RX_9:
      
      case MENU_ID_RX_SETTING_BIND:
        TX_MODE = MODE_BIND;
        break;
      case MENU_ID_RX_RENAME_OK:
        strcpy(rxRenameTemplate->ParentMenu, title);
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
    if (currentMenu->Prev) {
      currentMenu = currentMenu->Prev;
    }
    showMenu(currentMenu);  // print menu
  }
}

void showMenu(MenuNode* node) {
  if (node) {
    char* title;
    if (node->ParentMenu) {
      title = node->ParentMenu;
    } else {
      title = node->Title;
    }
    Display_refreshMenu(title, node->Items, node->ItemCount, node->Index);
  } else {
#ifdef DEBUG
    Serial.println(F("Menu not set!"));
#endif
  }
}

void showRxRename(char* rxName) {
  
}

void softReset() {
  asm volatile ("  jmp 0");
}
