#include "Arduino.h"
#include <EEPROM.h>
#include "types.h"
#include "config.h"
#include "def.h"
#include "EEPROM.h"

void EEPROM_ensureValid() {
  if (!EEPROM_checkVersionId()) {
    EEPROM_loadDefaults();
  }
}

bool EEPROM_checkVersionId()  {
  char versionId[EEPROM_VERSION_ID_LENGTH + 1];
  EEPROM.get(EEPROM_START_ADDRESS, versionId);
  versionId[EEPROM_VERSION_ID_LENGTH] = '\0'; // make a valid char array
#ifdef DEBUG
  Serial.print("EEPROM Version ID: ");
  Serial.println(versionId);
  Serial.print("EEPROM Version ID should be: ");
  Serial.println(EEPROM_VERSION_ID);
#endif
  return !strcmp(versionId, EEPROM_VERSION_ID);
}

uint8_t EEPROM_readRadioPaLevel() {
  uint8_t radioPaLevel;
  EEPROM.get(RADIO_PA_LEVEL_EEPROM_ADDRESS, radioPaLevel);
  return radioPaLevel;
}

uint8_t EEPROM_readCurrentRxId() {
  uint8_t currentRxId;
  EEPROM.get(CURRENT_RX_ID_EEPROM_ADDRESS, currentRxId);
  return currentRxId;
}

void EEPROM_writeCurrentRxId(uint8_t currentRxId) {
  EEPROM.put(CURRENT_RX_ID_EEPROM_ADDRESS, currentRxId);
}

RxConfig EEPROM_readRxConfig(uint8_t rxId) {
  RxConfig rxConfig;
  uint16_t rxConfigEepromAddress = RX_CONFIG_ALLOCATED_BYTES * (rxId - 1) + RX_CONFIG_EEPROM_START_ADDRESS;
  EEPROM.get(rxConfigEepromAddress, rxConfig);
  return rxConfig;
}

void EEPROM_writeRxConfig(RxConfig rxConfig) {
#ifdef DEBUG
  Serial.println(rxConfig.Name);
#endif
  uint16_t rxConfigEepromAddress = RX_CONFIG_ALLOCATED_BYTES * (rxConfig.Id - 1) + RX_CONFIG_EEPROM_START_ADDRESS;
  EEPROM.put(rxConfigEepromAddress, rxConfig);
}

void EEPROM_loadDefaults() {
#ifdef DEBUG
  Serial.print(F("Loading defaults for EEPROM..."));
#endif
  for (int i = 0 ; i < EEPROM_SETTING_LENGTH ; i++) {
    EEPROM.update(i, 0);
  }
  EEPROM.put(EEPROM_START_ADDRESS, EEPROM_VERSION_ID);
  EEPROM.put(RADIO_PA_LEVEL_EEPROM_ADDRESS, RADIO_PA_LEVEL_UPPER_BOUNDARY);
  EEPROM.put(CURRENT_RX_ID_EEPROM_ADDRESS, RADIO_RX_ID_LOWER_BOUNDARY);
  for (uint8_t i = RADIO_RX_ID_LOWER_BOUNDARY; i <= RADIO_RX_ID_UPPER_BOUNDARY; i++) {
    RxConfig rxConfig;
    rxConfig.Id = i;
    rxConfig.RadioChannel = RADIO_CHANNEL_LOWER_BOUNDARY;
    rxConfig.ThrottleTrim = 0;
    rxConfig.YawTrim = 0;
    rxConfig.PitchTrim = 0;
    rxConfig.RollTrim = 0;
    char rxName[10] = "RX ";
    char rxIdChar[3] = "";
    itoa(i, rxIdChar, 10);
    if (i < 10) {
      strcat(rxName, "0");
    }
    strcat(rxName, rxIdChar);
    strcat(rxName, "    ");
    strcpy(rxConfig.Name, rxName);
    uint16_t rxConfigEepromAddress = RX_CONFIG_ALLOCATED_BYTES * (i - 1) + RX_CONFIG_EEPROM_START_ADDRESS;
    EEPROM.put(rxConfigEepromAddress, rxConfig);
  }
#ifdef DEBUG
  Serial.println(F("Done"));
#endif
}

#ifdef DEBUG
void EEPROM_dumpAll() {
  Serial.println(F("EEPROM DATA:    "));
  for (int i = 0 ; i < 300 ; i++) {
    Serial.print("  ");
    Serial.print(i);
    Serial.print(F("=>"));
    Serial.print(EEPROM.read(i));
  }
  Serial.println("");
}
#endif
