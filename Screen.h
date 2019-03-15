#ifndef Screen_H_
#define Screen_H_

void Screen_showWelcomeScreen(char* projectName, char* projectVersion);
void Screen_showModeScreen(uint8_t txMode);
void Screen_showMenu(char* title, MenuNodeItem* items, uint8_t count, uint8_t index, void (*displayFunc)(uint8_t*));
void Screen_showMenuRxRename(uint8_t *lineCount);
void Screen_showControlScreen(ControlData controlData, RateData rateData, uint32_t currentTime);

extern MenuItemEdit itemEdit;
extern MenuPagination pagination;

#endif /* Screen_H_ */
