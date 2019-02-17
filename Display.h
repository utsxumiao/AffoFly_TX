#ifndef Display_H_
#define Display_H_

void Display_showWelcomeScreen(char* projectName, char* projectVersion);
void Display_showModeScreen(uint8_t txMode);
void Display_refreshMenu(char* title, char menuItems[][MENU_ITEM_LENGTH], uint8_t count, uint8_t currentMenuIndex);

#endif /* Display_H_ */
