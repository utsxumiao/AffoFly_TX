#ifndef Display_H_
#define Display_H_

void Screen_showWelcomeScreen(char* projectName, char* projectVersion);
void Screen_showModeScreen(uint8_t txMode);
void Screen_showMenu(char* title, MenuNodeItem* items, uint8_t count, uint8_t index, void (*displayFunc)(uint8_t*));
void Screen_showMenuRxRename(uint8_t *lineCount);

extern MenuItemEdit itemEdit;
extern MenuPagination pagination;

#endif /* Display_H_ */
