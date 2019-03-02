#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "MENU.h"
#include "Display.h"

#define MENU_ID_TOP_CONTROL         10
#define MENU_ID_TOP_SIMULATOR       11
#define MENU_ID_TOP_TRAINER         12
#define MENU_ID_TOP_STUDENT         13
#define MENU_ID_TOP_SETTING         14

#define MENU_ID_CONTROL_AUX         20
#define MENU_ID_CONTROL_TYPR        21
#define MENU_ID_CONTROL_MODE        22
#define MENU_ID_CONTROL_BATTERY     23
#define MENU_ID_CONTROL_SWD         24
#define MENU_ID_CONTROL_RX          25

#define MENU_ID_SETTING_TX          30
#define MENU_ID_SETTING_RX          31
#define MENU_ID_SETTING_DATA_RST    32
#define MENU_ID_SETTING_SOFT_RST    33

#define MENU_ID_RX_0                40
#define MENU_ID_RX_1                41
#define MENU_ID_RX_2                42
#define MENU_ID_RX_3                43
#define MENU_ID_RX_4                44
#define MENU_ID_RX_5                45
#define MENU_ID_RX_6                46
#define MENU_ID_RX_7                47
#define MENU_ID_RX_8                48
#define MENU_ID_RX_9                49

#define MENU_ID_RX_SETTING_SELECT   50
#define MENU_ID_RX_SETTING_RENAME   51
#define MENU_ID_RX_SETTING_BIND     52
#define MENU_ID_RX_SETTING_TOKEN    53
#define MENU_ID_RX_SETTING_CHANNEL  54

#define MENU_ID_RX_RENAME_OK        60

#define RX_NAME_MAX_LEN             13

// Top menu items
const char menu_top_title[] = "MODE";   // Menu title
const char menu_top_0[] = "CONTROL";
const char menu_top_1[] = "SIMULATOR";
const char menu_top_2[] = "TRAINER";
const char menu_top_3[] = "STUDENT";
const char menu_top_4[] = "SETTING";

// Control menu items
const char menu_control_title[] = "CONTROL";
const char menu_control_0[] = "AUX 1-N";    // when IsTrimming == false
const char menu_control_1[] = "TYPR Rate";  // when IsTrimming == true
const char menu_control_2[] = "MODE";
const char menu_control_3[] = "Battery";
const char menu_control_4[] = "SWD 1-2";
const char menu_control_5[] = "RX";

// Setting menu items
const char menu_setting_title[] = "SETTING";
const char menu_setting_0[] = "TX_UniqueId";
const char menu_setting_1[] = "RX Selection";
const char menu_setting_2[] = "Data Reset";
const char menu_setting_3[] = "Soft Reset/Restart";

// TX_UniqueId menu items


// RX meu items
const char menu_rx_title[] = "RX";
char menu_rx[][RX_NAME_MAX_LEN] = {"RX1 Name", "RX2 Name", "RX3 Name", "RX4 Name", "RX5 Name", "RX6 Name", "RX7 Name", "RX8 Name", "RX9 Name", "RX10 Name"};


// RX setting menu items
const char menu_rx_setting_0[] = "Select";
const char menu_rx_setting_1[] = "Rename";
const char menu_rx_setting_2[] = "Bind";
const char menu_rx_setting_3[] = "(Token)";
const char menu_rx_setting_4[] = "(Channel)";


MenuNode* topMenu;
MenuNode* currentMenu;
MenuNode* rxSettingTemplate;
MenuNode* rxRenameTemplate;

void Menu_init() {
  Menu_initButtons();
  setupMenu();
  showMenu(currentMenu);
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
  MenuNode* controlMenu;
  MenuNode* settingMenu;
  MenuNode* rxMenu;
  
  // Top menu --------------------
  topMenu = (MenuNode*)malloc(sizeof(MenuNode));
  initMenuNode(topMenu, menu_top_title, 5);
  initMenuNodeItem(topMenu->Items, 0, MENU_ID_TOP_CONTROL, menu_top_0);
  initMenuNodeItem(topMenu->Items, 1, MENU_ID_TOP_SIMULATOR, menu_top_1);
  initMenuNodeItem(topMenu->Items, 2, MENU_ID_TOP_TRAINER, menu_top_2);
  initMenuNodeItem(topMenu->Items, 3, MENU_ID_TOP_STUDENT, menu_top_3);
  initMenuNodeItem(topMenu->Items, 4, MENU_ID_TOP_SETTING, menu_top_4);

  currentMenu = topMenu;  // Top menu is seleted by default

  // Control menu --------------------
  controlMenu = (MenuNode*)malloc(sizeof(MenuNode));
  initMenuNode(controlMenu, menu_control_title, 6);
  initMenuNodeItemWithLookup(controlMenu->Items, 0, MENU_ID_CONTROL_AUX, menu_control_0);
  initMenuNodeItemWithLookup(controlMenu->Items, 1, MENU_ID_CONTROL_TYPR, menu_control_1);
  initMenuNodeItem(controlMenu->Items, 2, MENU_ID_CONTROL_MODE, menu_control_2);
  initMenuNodeItem(controlMenu->Items, 3, MENU_ID_CONTROL_BATTERY, menu_control_3);
  initMenuNodeItem(controlMenu->Items, 4, MENU_ID_CONTROL_SWD, menu_control_4);
  initMenuNodeItem(controlMenu->Items, 5, MENU_ID_CONTROL_RX, menu_control_5);
  controlMenu->Prev = topMenu;  // Link back to the previous menu node
  topMenu->Items[0].Item = controlMenu;  // Link with the menu item in the previous menu node

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
  initMenuNode(rxSettingTemplate, "", 5);
  initMenuNodeItem(rxSettingTemplate->Items, 0, MENU_ID_RX_SETTING_SELECT, menu_rx_setting_0);
  initMenuNodeItem(rxSettingTemplate->Items, 1, MENU_ID_RX_SETTING_RENAME, menu_rx_setting_1);
  initMenuNodeItem(rxSettingTemplate->Items, 2, MENU_ID_RX_SETTING_BIND, menu_rx_setting_2);
  initMenuNodeItem(rxSettingTemplate->Items, 3, MENU_ID_RX_SETTING_TOKEN, menu_rx_setting_3);
  initMenuNodeItem(rxSettingTemplate->Items, 4, MENU_ID_RX_SETTING_CHANNEL, menu_rx_setting_4);
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
  if (menuId >= MENU_ID_RX_0 && menuId <= MENU_ID_RX_9) { // RX Name item has been selected
    rxSettingTemplate->ParentId = menuId;
    rxSettingTemplate->ParentMenu = title;
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
      case MENU_ID_CONTROL_AUX:
//        Serial.println("Action: Control -> AUX");
        break;
      case MENU_ID_CONTROL_TYPR:
//        Serial.println("Action: Control -> TYPR");
        break;
      case MENU_ID_CONTROL_MODE:
//        Serial.println("Action: Control -> MODE");
        break;
      case MENU_ID_CONTROL_BATTERY:
//        Serial.println("Action: Control -> BATTERY");
        break;
      case MENU_ID_CONTROL_SWD:
//        Serial.println("Action: Control -> SWD");
        break;
      case MENU_ID_CONTROL_RX:
//        Serial.println("Action: Control -> RX");
        break;
      case MENU_ID_RX_RENAME_OK:
        strcpy(rxRenameTemplate->ParentMenu, title);
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

void showMenu(MenuNode* node)  {
  if (node) {
    Serial.print("== ");
    if (node->ParentMenu) {
      Serial.print(node->ParentMenu);
    }
    else  {
      Serial.print(node->Title);
    }
    Serial.println(" =="); // print Title first

    bool lookupData = true;
    int j = 0;
    
    for (int i = 0; i < node->ItemCount; i++) {
      lookupData = true;

      if (lookupData) {
        if (node->Index == j) {
          Serial.print("*");
        }
        else  {
          Serial.print(" ");
        }
        j++;
        
        Serial.println(node->Items[i].Menu); // print each menu item
      }
    }
    node->LookupItemCount = j;  // Refresh the number of items in a node
  }
  else  {
    Serial.println("Menu not set!");
  }
  Serial.println(""); // print an empty line
}

void showRxRename(char* rxName) {
  //handleMenu(MENU_ID_RX_RENAME_OK, "RX3 Default");  // testing to change RX name here..
}






























//========================================================================================

//#ifdef BUDDY
//#define ROOT_MENU_ITEM_COUNT  5
//#else
//#define ROOT_MENU_ITEM_COUNT  3
//#endif
//
//char title[MENU_TITLE_LENGTH] = "TX MODE";
//char rootItems[ROOT_MENU_ITEM_COUNT][MENU_ITEM_LENGTH] = {
//  "CONTROL",
//  "SIMULATOR",
//#ifdef BUDDY
//  "TRAINER",
//  "STUDENT",
//#endif
//  "SETTING"
//};
//
//uint8_t currentMenuIndex = 0;
//
//void Menu_init() {
//  Menu_initButtons();
//  showScreen();
//}
//
//void Menu_initButtons() {
//  Bounce backBounce = Bounce();
//  backBounce.attach(BUTTON_BACK_PIN, INPUT_PULLUP);
//  backBounce.interval(5);
//  bounces[1] = backBounce;
//  Bounce goBounce = Bounce();
//  goBounce.attach(BUTTON_GO_PIN, INPUT_PULLUP);
//  goBounce.interval(5);
//  bounces[1] = goBounce;
//  Bounce prevBounce = Bounce();
//  prevBounce.attach(BUTTON_PREV_PIN, INPUT_PULLUP);
//  prevBounce.interval(5);
//  bounces[2] = prevBounce;
//  Bounce nextBounce = Bounce();
//  nextBounce.attach(BUTTON_NEXT_PIN, INPUT_PULLUP);
//  nextBounce.interval(5);
//  bounces[3] = nextBounce;
//}
//
//void Menu_checkButtons() {
//  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
//    bounces[i].update();
//    if (bounces[i].fell()) {
//      switch (i) {
//        case 0:
//          back();
//          break;
//        case 1:
//          go();
//          break;
//        case 2:
//          prev();
//          break;
//        case 3:
//          next();
//          break;
//        default:
//          break;
//      }
//    }
//  }
//}
//
//void back() {
//  //TODO:
//}
//
//void go() {
//  TX_MODE = currentMenuIndex;
//}
//
//void prev() {
//  if (currentMenuIndex <= 0) {
//    currentMenuIndex = ROOT_MENU_ITEM_COUNT;
//  }
//  currentMenuIndex--;
//  showScreen();
//}
//
//void next() {
//  if (currentMenuIndex >= ROOT_MENU_ITEM_COUNT - 1) {
//    currentMenuIndex = 0;
//  } else {
//    currentMenuIndex++;
//  }
//  showScreen();
//}
//
//void showScreen() {
//  Display_refreshMenu(title, rootItems, ROOT_MENU_ITEM_COUNT, currentMenuIndex);
//}
