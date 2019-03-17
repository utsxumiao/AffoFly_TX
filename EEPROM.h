#ifndef EEPROM_H_
#define EEPROM_H_

void EEPROM_ensureValid();
uint8_t EEPROM_readRadioPaLevel();
uint8_t EEPROM_readCurrentRxId();
void EEPROM_writeCurrentRxId(uint8_t currentRxId);
RxConfig EEPROM_readRxConfig(uint8_t rxId);
void EEPROM_writeRxConfig(RxConfig rxConfig);
void EEPROM_loadDefaults();
bool EEPROM_checkVersionId();
#ifdef DEBUG
void EEPROM_dumpAll();
#endif

#endif /* EEPROM_H_ */
