#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "EEPROM.h"
#include "Bounce2.h"
#include "Menu.h"
#include "AffoFly_Transmitter.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"
#include "Radio.h"

/**************************************************************************************/
/***************                       NRF24                       ********************/
/**************************************************************************************/
#ifdef NRF24

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
uint8_t radioPaLevel = RF24_PA_MAX;
MenuNode* bindMenu = (MenuNode*)malloc(sizeof(MenuNode));

void Radio_init() {
  radio.begin();
  radio.setPALevel(radioPaLevel);
  radio.setAutoAck(false);
  radio.setChannel(CURRENT_RX_CONFIG.RadioChannel);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(0xE8E8F0F0E1LL);
  radio.startListening();
  radio.stopListening();
#ifdef DEBUG
  Serial.print(F("CHANNEL          = ")); Serial.println(CURRENT_RX_CONFIG.RadioChannel);
  printf_begin();
  radio.printDetails();
#endif
}

void Radio_sendData(ControlData controlData) {
//#ifdef DEBUG
//  Serial.print("UniqueId: ");     Serial.print(controlData.UniqueId);
//  Serial.print("    Throttle: "); Serial.print(controlData.Throttle);
//  Serial.print("    Yaw: ");      Serial.print(controlData.Yaw);
//  Serial.print("    Pitch: ");    Serial.print(controlData.Pitch);
//  Serial.print("    Roll: ");     Serial.print(controlData.Roll);
//  Serial.print("    Aux1: ");     Serial.print(controlData.Aux1);
//  Serial.print("    Aux2: ");     Serial.print(controlData.Aux2);
//  Serial.print("    Aux3: ");     Serial.print(controlData.Aux3);
//  Serial.print("    Aux4: ");     Serial.print(controlData.Aux4);
//  Serial.print("    Aux5: ");     Serial.print(controlData.Aux5);
//  Serial.print("    Aux6: ");     Serial.print(controlData.Aux6);
//  Serial.print("    Swd1: ");     Serial.print(controlData.Swd1);
//  Serial.print("    Swd2: ");     Serial.print(controlData.Swd2);
//  Serial.println();
//#endif
  radio.write(&controlData, sizeof(ControlData));
}

void Radio_bind() {
  initMenuNode(bindMenu, "BIND", 3);
  bindMenu->Index = -1;
  initMenuNodeItem(bindMenu->Items, 0, 0, "Channel: ..."); 
  initMenuNodeItem(bindMenu->Items, 1, 0, "");
  initMenuNodeItem(bindMenu->Items, 2, 0, "");
  //Display_refreshMenu(bindMenu->Title, bindMenu->Items, bindMenu->ItemCount, bindMenu->Index);
  showMenu(bindMenu);
  uint8_t channel = findChannel();
  char strChannel[4];
  initMenuNodeItem(bindMenu->Items, 0, 0, strcat("Channel: ", itoa(channel, strChannel, 10))); 
  showMenu(bindMenu);
  char* token = generateToken();
  initMenuNodeItem(bindMenu->Items, 1, 0, "Token ready.");
  initMenuNodeItem(bindMenu->Items, 2, 0, "Waiting RX...");
  showMenu(bindMenu);
  bindRx(channel, token);
}

uint8_t findChannel() {
  delay(2000);
  return 120;
}

char * generateToken() {
#ifdef V_BAT
  //use battery reading to seed ramdom
#else
  initMenuNodeItem(bindMenu->Items, 1, 0, "Move Throttle");
  showMenu(bindMenu);
#endif
  return "aaaa";
}

void bindRx(uint8_t channel, char* token) {
  radio.begin();
  radio.setPALevel(radioPaLevel);
  radio.setAutoAck(false);
  radio.setChannel(channel);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(0xE8E8F0F0E1LL);
  radio.startListening();
  radio.stopListening();
#ifdef DEBUG
  Serial.print(F("CHANNEL          = ")); Serial.println(channel);
  printf_begin();
  radio.printDetails();
#endif
  bool bound = false;
  while(!bound) {
    bool txSent = radio.write(&token, sizeof(token));
    if(txSent && radio.isAckPayloadAvailable()) {
      //update RX data in EEPROM
      bound = true;
      initMenuNodeItem(bindMenu->Items, 2, 0, "Bound!");
      showMenu(bindMenu);
    }
  }

}

#endif

/**************************************************************************************/
/***************                    OTHER RADIO                    ********************/
/**************************************************************************************/
