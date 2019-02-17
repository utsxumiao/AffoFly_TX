#ifndef EEPROM_H_
#define EEPROM_H_

void EEPROM_ensureValid();
uint16_t EEPROM_readRadioUniqueId();
uint8_t EEPROM_readRadioPaLevel();
uint8_t EEPROM_readCurrentRxId();
RxConfig EEPROM_readRxConfig(uint8_t rxId);
void EEPROM_writeRxConfig(RxConfig rxConfig);
void EEPROM_loadDefaults();
#ifdef DEBUG
void EEPROM_dumpAll();
#endif

#endif /* EEPROM_H_ */
