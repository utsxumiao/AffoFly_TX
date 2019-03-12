#ifndef MENU_H_
#define MENU_H_


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

#define ITEM_EDIT_NOT_SELECTED      100


void Menu_init();
void Menu_initButtons();
void Menu_checkButtons();
void setupMenu();
void setPagination(uint8_t scrollIndex);
void initMenuNode(MenuNode* node, char* title, uint8_t itemCount);
void initMenuNodeItem(MenuNodeItem* items, uint8_t index, uint8_t id, char* menu);
void initMenuNodeItemWithSelectable(MenuNodeItem* items, uint8_t index, uint8_t id, char* menu, bool selectable);
void navigateMenu(MenuNode* node, int8_t upOrDown);
uint8_t getNextAllowedChar(uint8_t ch, int8_t upOrDown);
MenuNodeItem* getCurrentMenuNodeItem(MenuNode* node);
char* getRxName(uint8_t index);
void selectMenu();
void handleMenu(uint8_t menuId, char* title);
void goBackMenu();
void showMenu(MenuNode* node);
void showMenu(MenuNode* node, void (*displayFunc)(uint8_t*));
void showMenuItemEditMode(MenuNode* node);
void showRxRename(bool saveMode);
void softReset();

#endif /* MENU_H_ */
