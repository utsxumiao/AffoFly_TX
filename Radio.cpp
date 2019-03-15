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

uint32_t previousRadioSendTime = 0;
static const uint16_t radioSendInterval = 10000; // 100p/s


/**************************************************************************************/
/***************                       NRF24                       ********************/
/**************************************************************************************/
#ifdef NRF24

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
uint8_t radioPaLevel = RF24_PA_MAX;
MenuNode* bindMenu = (MenuNode*)malloc(sizeof(MenuNode));

#ifdef SHOW_RATE
uint16_t RADIO_COUNT;
#endif

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

void Radio_sendData(ControlData controlData, uint32_t currentTime) {
  if (currentTime - previousRadioSendTime >= radioSendInterval) {
    previousRadioSendTime = currentTime;
    //#ifdef DEBUG
    //  Serial.print("Token: ");     Serial.print(controlData.Token);
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
    controlData.Token = CURRENT_RX_CONFIG.Token;
    radio.write(&controlData, sizeof(ControlData));
#ifdef SHOW_RATE
    RADIO_COUNT++;
#endif
  }
}

void Radio_bind() {
  radio.begin();
  radio.setAutoAck(false);
  radio.startListening();
  radio.stopListening();

  initMenuNode(bindMenu, "BIND", 3);
  bindMenu->Index = -1;
  initMenuNodeItem(bindMenu->Items, 0, 0, "Channel: ...");
  initMenuNodeItem(bindMenu->Items, 1, 0, "");
  initMenuNodeItem(bindMenu->Items, 2, 0, "");
  showMenu(bindMenu);
  uint8_t channel = findChannel();
  char strChannel[4];
  initMenuNodeItem(bindMenu->Items, 0, 0, strcat("Channel: ", itoa(channel, strChannel, 10)));
  showMenu(bindMenu);
  uint32_t token = generateToken();
  initMenuNodeItem(bindMenu->Items, 1, 0, "Token ready.");
  initMenuNodeItem(bindMenu->Items, 2, 0, "Waiting RX...");
  showMenu(bindMenu);
  bindRx(channel, token);
}

uint8_t findChannel() {
  uint8_t loads[RADIO_CHANNEL_UPPER_BOUNDARY - RADIO_CHANNEL_LOWER_BOUNDARY];
  for (int8_t i = 0; i < RADIO_SCAN_REPEATS; i++) {
    for (int8_t j = RADIO_CHANNEL_LOWER_BOUNDARY; j <= RADIO_CHANNEL_UPPER_BOUNDARY; j++) {
      radio.setChannel(j);
      radio.startListening();
      delayMicroseconds(128);
      radio.stopListening();
      if ( radio.testCarrier() ) {
        loads[j]++;
      }
    }
  }
#ifdef DEBUG
  for (int8_t i = RADIO_CHANNEL_LOWER_BOUNDARY; i <= RADIO_CHANNEL_UPPER_BOUNDARY; i++) {
    Serial.print(loads[i]);   Serial.print("    ");
  }
  Serial.println();
#endif
  uint8_t minValue = RADIO_SCAN_REPEATS;
  for (int8_t i = RADIO_CHANNEL_LOWER_BOUNDARY; i <= RADIO_CHANNEL_UPPER_BOUNDARY; i++) {
    if (loads[i] == 0) {
      return i;
    } else {
      if (loads[i] < minValue) {
        minValue = loads[i];
      }
    }
  }
  return minValue;
}

uint32_t generateToken() {
  return micros();
}

void bindRx(uint8_t channel, uint32_t token) {
  radio.setPALevel(radioPaLevel);
  radio.setChannel(channel);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(RADIO_PIPE);
#ifdef DEBUG
  Serial.print(F("CHANNEL          = ")); Serial.println(channel);
  printf_begin();
  radio.printDetails();
#endif
  bool bound = false;
  TxBindData txBindData;
  strcpy(txBindData.TxIdentifier, TX_IDENTIFIER);
  txBindData.Token = token;
  while (!bound) {
    bool txSent = radio.write(&txBindData, sizeof(txBindData));
    if (txSent && radio.isAckPayloadAvailable()) {
      RxBindData rxBindData;
      radio.read(&rxBindData, sizeof(rxBindData));
      if (rxBindData.RxIdentifier == TX_IDENTIFIER && rxBindData.Token == token) {
        //update RX data in EEPROM
        bound = true;
        initMenuNodeItem(bindMenu->Items, 2, 0, "Bound!");
        showMenu(bindMenu);
      }
    }
    delayMicroseconds(20000); //Place some delay to avoid flodding the channel
  }
}

#endif

/**************************************************************************************/
/***************                    OTHER RADIO                    ********************/
/**************************************************************************************/
