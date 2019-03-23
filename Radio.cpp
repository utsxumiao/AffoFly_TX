#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "EEPROM.h"
#include "Bounce2.h"
#include "Buzzer.h"
#include "AffoFly_Transmitter.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "Menu.h"
#include "Screen.h"
#include "Radio.h"

uint32_t previousRadioSendTime = 0;
static const uint16_t radioSendInterval = 5000; // 200p/s


/**************************************************************************************/
/***************                       NRF24                       ********************/
/**************************************************************************************/
#ifdef NRF24

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

#ifdef SHOW_RATE
uint16_t RADIO_COUNT;
#endif

void Radio_init() {
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setAutoAck(false);
  radio.setChannel(CURRENT_RX_CONFIG.RadioChannel);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(RADIO_PIPE);
  radio.startListening();
  radio.stopListening();
#ifdef DEBUG
  Serial.print(F("CHANNEL          = ")); Serial.println(CURRENT_RX_CONFIG.RadioChannel);
  radio.printDetails();
#endif
}

void Radio_sendData(ControlData controlData, uint32_t currentTime) {
  if (currentTime - previousRadioSendTime >= radioSendInterval) {
    previousRadioSendTime = currentTime;
    controlData.Token = CURRENT_RX_CONFIG.Token;
    //#ifdef DEBUG
//      Serial.print("Token: ");     Serial.print(controlData.Token);
//      Serial.print("    Throttle: "); Serial.print(controlData.Throttle);
//      Serial.print("    Yaw: ");      Serial.print(controlData.Yaw);
//      Serial.print("    Pitch: ");    Serial.print(controlData.Pitch);
//      Serial.print("    Roll: ");     Serial.print(controlData.Roll);
//      Serial.print("    Aux1: ");     Serial.print(controlData.Aux1);
//      Serial.print("    Aux2: ");     Serial.print(controlData.Aux2);
//      Serial.print("    Aux3: ");     Serial.print(controlData.Aux3);
//      Serial.print("    Aux4: ");     Serial.print(controlData.Aux4);
//      Serial.print("    Aux5: ");     Serial.print(controlData.Aux5);
//      Serial.print("    Aux6: ");     Serial.print(controlData.Aux6);
//      Serial.print("    Swd1: ");     Serial.print(controlData.Swd1);
//      Serial.print("    Swd2: ");     Serial.print(controlData.Swd2);
//      Serial.println();
    //#endif
    radio.write(&controlData, sizeof(ControlData));
#ifdef SHOW_RATE
    RADIO_COUNT++;
#endif
  }
}

void Radio_bind() {
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.enableAckPayload();

  Screen_showRadioBindingScreen(0, false, false);
  uint8_t channel = findChannel();
  Screen_showRadioBindingScreen(channel, false, false);
  uint32_t token = generateToken();
  Screen_showRadioBindingScreen(channel, true, false);
  bindRx(channel, token);
  Screen_showRadioBindingScreen(channel, true, true);
}

uint8_t findChannel() {
  radio.startListening();
  radio.stopListening();
  uint16_t loads[RADIO_CHANNEL_UPPER_BOUNDARY - RADIO_CHANNEL_LOWER_BOUNDARY + 1];
  for (uint8_t i = 0; i < RADIO_SCAN_REPEATS; i++) {
    for (uint8_t j = RADIO_CHANNEL_LOWER_BOUNDARY; j <= RADIO_CHANNEL_UPPER_BOUNDARY; j++) {
      radio.setChannel(j);
      radio.startListening();
      delayMicroseconds(128);
      radio.stopListening();
      if ( radio.testCarrier() ) {
        loads[j - RADIO_CHANNEL_LOWER_BOUNDARY]++;
      }
    }
  }
#ifdef DEBUG
  for (int8_t i = RADIO_CHANNEL_LOWER_BOUNDARY; i <= RADIO_CHANNEL_UPPER_BOUNDARY; i++) {
    Serial.print(i);  Serial.print(F("=>"));  Serial.print(loads[i - RADIO_CHANNEL_LOWER_BOUNDARY]);   Serial.print("    ");
  }
  Serial.println();
#endif
  uint16_t minValue = 65000;
  for (int8_t i = RADIO_CHANNEL_LOWER_BOUNDARY; i <= RADIO_CHANNEL_UPPER_BOUNDARY; i++) {
    if (loads[i - RADIO_CHANNEL_LOWER_BOUNDARY] == 0) {
      return i;
    } else {
      if (loads[i - RADIO_CHANNEL_LOWER_BOUNDARY] < minValue) {
        minValue = loads[i - RADIO_CHANNEL_LOWER_BOUNDARY];
      }
    }
  }
  return minValue;
}

uint32_t generateToken() {
  return micros();
}

void bindRx(uint8_t channel, uint32_t token) {
  radio.setChannel(channel);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(RADIO_PIPE);
  radio.startListening();
  radio.stopListening();
#ifdef DEBUG
  Serial.print(F("Channel: ")); Serial.println(channel);
  radio.printDetails();
#endif
  const uint16_t radioSendInterval = 10000; // 100p/s
  uint32_t previousRadioSendTime = 0;

  bool bound = false;
  TxBindData txBindData;
  strcpy(txBindData.TxIdentifier, TX_IDENTIFIER);
  txBindData.Token = token;
#ifdef BUZZER
  BuzzerBeepPattern buzzerPattern = radioBinding;
  Buzzer_start(buzzerPattern);
#endif
  while (!bound) {
    uint32_t currentTime  = micros();
#ifdef BUZZER
    Buzzer_beep(currentTime);
#endif
    if (currentTime - previousRadioSendTime >= radioSendInterval) {
      previousRadioSendTime = currentTime;
      if (radio.write(&txBindData, sizeof(TxBindData))) {
        if (radio.isAckPayloadAvailable()) {
          char rxIdentifier[10];
          radio.read(&rxIdentifier, 10);
          if (strcmp(rxIdentifier, RX_IDENTIFIER) == 0) {
            selectedRxConfig.RadioChannel = channel;
            selectedRxConfig.Token = token;
            EEPROM_writeRxConfig(selectedRxConfig);
            bound = true;
          }
        }
      }
    }
  }
#ifdef BUZZER
  Buzzer_stop();
#endif
}

#endif

/**************************************************************************************/
/***************                    OTHER RADIO                    ********************/
/**************************************************************************************/
