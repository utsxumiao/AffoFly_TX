#ifndef Display_H_
#define Display_H_

void Display_showWelcomeScreen(char* projectName, char* projectVersion);
void Display_showModeScreen(uint8_t txMode);
void Display_refreshMenu(char* title, MenuNodeItem* items, uint8_t count, uint8_t index);

#endif /* Display_H_ */
