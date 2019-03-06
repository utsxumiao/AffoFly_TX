#ifndef MENU_H_
#define MENU_H_

void Menu_init();
void Menu_initButtons();
void Menu_checkButtons();
void setupMenu();
void initMenuNode(MenuNode* node, char* title, uint8_t itemCount);
void initMenuNodeItem(MenuNodeItem* items, uint8_t index, uint8_t id, char* menu);
void initMenuNodeItemWithLookup(MenuNodeItem* items, uint8_t index, uint8_t id, char* menu);
void navigateMenu(MenuNode* node, int8_t upOrDown);
void selectMenu();
void handleMenu(uint8_t menuId, char* title);
void goBackMenu();
void showMenu(MenuNode* node);
void showRxRename(char* rxName);
void softReset();

#endif /* MENU_H_ */
