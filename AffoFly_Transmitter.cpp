#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "printf.h"
#include "Battery.h"
#include "Buzzer.h"
#include "CPPM.h"
#include "EEPROM.h"
#include "Menu.h"
#include "Screen.h"
#include "Radio.h"


/**************************************************************************************/
/***************              General Global Variables             ********************/
/**************************************************************************************/
uint8_t PREVIOUS_TX_MODE = 255; //The initial value which should not be any valid MODE
uint8_t TX_MODE;
RxConfig CURRENT_RX_CONFIG;
bool trimming = false;
int8_t trimmingStickIndex = 0;
uint8_t trimmingSteps = 2;
uint8_t aux1State = 0;
uint8_t aux2State = 0;
uint8_t aux3State = 0;
uint8_t aux4State = 0;
//uint8_t previousGroupDValue;

#ifdef EXTEND_CHANNEL
uint8_t aux5State = 0;
uint8_t aux6State = 0;
//uint8_t previousGroupBValue;
#endif

Bounce bounces[BUTTON_COUNT];

// Loop time control
uint32_t currentTime;

static const uint16_t mainLoopInterval = 500;
uint32_t previousMainLoopTime = 0;
static const uint16_t buttonCheckInterval = 5000;
uint32_t previousButtonCheckTime = 0;
//uint8_t buttonCheckInterval = 5; //5ms for debounce, bouncing pretty much dies out after 4ms.
static const uint16_t getDataInterval = 10000;
uint32_t previousGetDataTime = 0;

ControlData controlData;

#ifdef SHOW_RATE
static const uint32_t rateRefreshInterval = 1000000;
uint32_t previousRateRefreshTime = 0;
uint16_t loopCount = 0;
uint16_t buttonCheckCount = 0;
uint16_t dataGetCount = 0;

RateData rateData;
#endif

void setup() {
#if defined(DEBUG) || defined(SCREEN_MOCK_SERIAL_MONITOR) || defined(CHECK_FREE_MEMORY)
  Serial.begin(115200);
  printf_begin(); // printf and sprintf prints using Serial.write()
#endif
  EEPROM_ensureValid();
#ifdef DEBUG
  EEPROM_dumpAll();
#endif
  Screen_init();
#ifdef V_BAT
  Battery_init();
#endif
#ifdef BUZZER
  Buzzer_init();
#endif
  TX_MODE = MODE_CONTROL;
  pinMode(FN_PIN, INPUT_PULLUP);
  if (digitalRead(FN_PIN) == LOW) {
    TX_MODE = MODE_MENU;
  }
#ifdef SHOW_WELCOME_SCREEN
  Screen_showWelcomeScreen(PROJECT_NAME, PROJECT_VERSION);
  delay(2000);
#endif
}

void loop() {
  if (TX_MODE != PREVIOUS_TX_MODE) {
    txModeInit();
    PREVIOUS_TX_MODE = TX_MODE;
  }
  uint32_t currentTime = micros();
  txModeProcess(currentTime);
#ifdef SHOW_RATE
  loopCount++;
  if (currentTime - previousRateRefreshTime >= rateRefreshInterval) {
    previousRateRefreshTime = currentTime;
    rateData.LoopRate = loopCount;
    rateData.ButtonCheckRate = buttonCheckCount;
    rateData.DataGetRate = dataGetCount;
    rateData.RadioRate = RADIO_COUNT;
    rateData.CPPMRate = CPPM_COUNT;
    
    loopCount = 0;
    buttonCheckCount = 0;
    dataGetCount = 0;
    RADIO_COUNT = 0;
    CPPM_COUNT = 0;
  }
#endif
}

void txModeInit() {
#ifdef DEBUG
  Serial.print(F("TX MODE: "));
  Serial.println(TX_MODE);
#endif

  // deallocate all resource for menu
  Menu_stop();

#ifdef SHOW_MODE_SCREEN
  Screen_showModeScreen(TX_MODE);
  delay(1000);
#endif

  switch (TX_MODE) {
    case MODE_CONTROL:
      Control_init();
      Radio_init();
      break;
    case MODE_MENU:
      Menu_init();
      break;
#ifdef SIMULATOR
    case MODE_SIMULATOR:
      Control_init();
      CPPM_init();
      break;
#endif
#ifdef BUDDY
    case MODE_TRAINER:
      Control_init();
      CPPM_init();
      break;
    case MODE_STUDENT:
      Control_init();
      Radio_init();
      break;
#endif
    case MODE_BIND:
      Radio_bind();
      break;
  }
}

void txModeProcess(uint32_t currentTime) {
  switch (TX_MODE) {
    case MODE_CONTROL:
      Control_checkButtons(currentTime);
      Control_getData(currentTime, false);
      Radio_sendData(controlData, currentTime);
      Screen_showControlScreen(controlData, rateData, trimming, trimmingStickIndex, currentTime, false);
      break;
#ifdef SIMULATOR
    case MODE_SIMULATOR:
      Control_checkButtons(currentTime);
      Control_getData(currentTime, false);
      CPPM_outputData(controlData);
      Screen_showControlScreen(controlData, rateData, trimming, trimmingStickIndex, currentTime, false);
      break;
#endif
#ifdef BUDDY
    case MODE_TRAINER:
      Control_checkButtons(currentTime);
      //TODO: Activate button flip then getControlData();CPPM_outputData(controlData);
      break;
    case MODE_STUDENT:
      Control_checkButtons(currentTime);
      //TODO: If receiving Trainer data, getTrainerData();sendRadioData(controlData);
      break;
#endif
    case MODE_MENU:
      Menu_checkButtons();
      break;
    case MODE_BIND:
      //Bind workflow is self-handled
      break;
  }
#ifdef V_BAT
  Battery_read(currentTime);
#endif
#ifdef BUZZER
  Buzzer_beep(currentTime);
#endif
}

void Control_init() {
  Control_initConfig();
  Control_initButtons();
  Control_resetData();
}

void Control_initConfig() {
  CURRENT_RX_CONFIG = EEPROM_readRxConfig(EEPROM_readCurrentRxId());
}

//void Control_initButtons() {
//  pinMode(FN_PIN, INPUT_PULLUP);
//  if (AUX_1_STATES > 1) pinMode(AUX1_PIN, INPUT_PULLUP);
//  if (AUX_2_STATES > 1) pinMode(AUX2_PIN, INPUT_PULLUP);
//  if (AUX_3_STATES > 1) pinMode(AUX3_PIN, INPUT_PULLUP);
//  if (AUX_4_STATES > 1) pinMode(AUX4_PIN, INPUT_PULLUP);
//#ifdef EXTEND_CHANNEL
//  if (AUX_5_STATES > 1) pinMode(AUX5_PIN, INPUT_PULLUP);
//  if (AUX_6_STATES > 1) pinMode(AUX6_PIN, INPUT_PULLUP);
//  previousGroupDValue = PIND & B11111100;
//  previousGroupBValue = PINB & B00000001;
//#else
//  previousGroupDValue = PIND & B10111100;
//#endif
//
//}
void Control_initButtons() {
  Bounce fnBounce = Bounce();
  fnBounce.attach(FN_PIN, INPUT_PULLUP);
  fnBounce.interval(BUTTON_DEBOUNCE_MS);
  bounces[0] = fnBounce;
  if (AUX_1_STATES > 1) {
    Bounce aux1Bounce = Bounce();
    aux1Bounce.attach(AUX1_PIN, INPUT_PULLUP);
    aux1Bounce.interval(BUTTON_DEBOUNCE_MS);
    bounces[1] = aux1Bounce;
  }
  if (AUX_2_STATES > 1) {
    Bounce aux2Bounce = Bounce();
    aux2Bounce.attach(AUX2_PIN, INPUT_PULLUP);
    aux2Bounce.interval(BUTTON_DEBOUNCE_MS);
    bounces[2] = aux2Bounce;
  }
  if (AUX_3_STATES > 1) {
    Bounce aux3Bounce = Bounce();
    aux3Bounce.attach(AUX3_PIN, INPUT_PULLUP);
    aux3Bounce.interval(BUTTON_DEBOUNCE_MS);
    bounces[3] = aux3Bounce;
  }
  if (AUX_4_STATES > 1) {
    Bounce aux4Bounce = Bounce();
    aux4Bounce.attach(AUX4_PIN, INPUT_PULLUP);
    aux4Bounce.interval(BUTTON_DEBOUNCE_MS);
    bounces[4] = aux4Bounce;
  }
#ifdef EXTEND_CHANNEL
  if (AUX_5_STATES > 1) {
    Bounce aux5Bounce = Bounce();
    aux5Bounce.attach(AUX5_PIN, INPUT_PULLUP);
    aux5Bounce.interval(BUTTON_DEBOUNCE_MS);
    bounces[5] = aux5Bounce;
  }
  if (AUX_6_STATES > 1) {
    Bounce aux6Bounce = Bounce();
    aux6Bounce.attach(AUX6_PIN, INPUT_PULLUP);
    aux6Bounce.interval(BUTTON_DEBOUNCE_MS);
    bounces[6] = aux6Bounce;
  }
#endif
}

void Control_resetData() {
  controlData.Throttle = 1000;
  controlData.Yaw = 1500;
  controlData.Pitch = 1500;
  controlData.Roll = 1500;
  controlData.Aux1 = 1000;
  controlData.Aux2 = 1000;
  controlData.Aux3 = 1000;
  controlData.Aux4 = 1000;
  controlData.Aux5 = 1000;
  controlData.Aux6 = 1000;
  controlData.Swd1 = 1000;
  controlData.Swd2 = 1000;
}

void Control_checkButtons(uint32_t currentTime) {
  //  if (currentMillis - previousButtonCheckMillis >= buttonCheckInterval) {
  //    previousButtonCheckMillis = currentMillis;
  //#ifdef EXTEND_CHANNEL
  //    uint8_t groupDValue = PIND & B11111100;
  //    uint8_t groupBValue = PINB & B00000001;
  //#else
  //    uint8_t groupDValue = PIND & B10111100;
  //    if (previousGroupDValue != groupDValue) {
  //
  //    }
  //#endif
  //  }
  if (currentTime - previousButtonCheckTime >= buttonCheckInterval) {
    previousButtonCheckTime = currentTime;
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
      bounces[i].update();
      if (bounces[i].fell()) {
        switch (i) {
          case 0:
            trimming = !trimming;
            trimmingStickIndex = 0;
            break;
          case 1:
            if (trimming) {
              trimStick(false);
            } else {
              if (AUX_1_STATES > 1) {
                aux1State++;
                if (aux1State > AUX_1_STATES - 1) {
                  aux1State = 0;
                }
              }
            }
            break;
          case 2:
            if (trimming) {
              trimStick(true);
            } else {
              if (AUX_2_STATES > 1) {
                aux2State++;
                if (aux2State > AUX_2_STATES - 1) {
                  aux2State = 0;
                }
              }
            }
            break;
          case 3:
            if (trimming) {
              trimmingStickIndex++;
              if (trimmingStickIndex > 3) {
                trimmingStickIndex = 0;
              }
            } else {
              if (AUX_3_STATES > 1) {
                aux3State++;
                if (aux3State > AUX_3_STATES - 1) {
                  aux3State = 0;
                }
              }
            }
            break;
          case 4:
            if (trimming) {
              trimmingStickIndex--;
              if (trimmingStickIndex < 0) {
                trimmingStickIndex = 3;
              }
            } else {
              if (AUX_4_STATES > 1) {
                aux4State++;
                if (aux4State > AUX_4_STATES - 1) {
                  aux4State = 0;
                }
              }
            }
            break;
#ifdef EXTEND_CHANNEL
          case 5:
            if (AUX_5_STATES > 1 && !trimming) {
              aux5State++;
              if (aux5State > AUX_5_STATES - 1) {
                aux5State = 0;
              }
            }
            break;
          case 6:
            if (AUX_6_STATES > 1 && !trimming) {
              aux6State++;
              if (aux6State > AUX_6_STATES - 1) {
                aux6State = 0;
              }
            }
            break;
#endif
        }
#ifdef BUZZER
        BuzzerBeepPattern buzzerPattern = buttonPress;
        Buzzer_start(buzzerPattern);
#endif
        Control_getData(currentTime, true);
        Screen_showControlScreen(controlData, rateData, trimming, trimmingStickIndex, currentTime, true);
        
#ifdef DEBUG
        Serial.print(F("Button pressed, index: "));
        Serial.print(i);
        Serial.print(F("  Aux1:"));
        Serial.print(aux1State);
        Serial.print(F("  Aux2:"));
        Serial.print(aux2State);
        Serial.print(F("  Aux3:"));
        Serial.print(aux3State);
        Serial.print(F("  Aux4:"));
        Serial.print(aux4State);
#ifdef EXTEND_CHANNEL
        Serial.print(F("  Aux5:"));
        Serial.print(aux5State);
        Serial.print(F("  Aux6:"));
        Serial.print(aux6State);
#endif
        if (trimming) {
          Serial.print(F("    Trimming Stick Index:"));
          Serial.print(trimmingStickIndex);
        }
        Serial.println();
#endif
      }
    }
#ifdef SHOW_RATE
    buttonCheckCount++;
#endif
  }
}

void Control_getData(uint32_t currentTime, bool forceExecute) {
  if (forceExecute || currentTime - previousGetDataTime >= getDataInterval) {
    previousGetDataTime = currentTime;
    //TODO: Reverse channel setting should be considered
    //TODO: Use port manipulation for better performance
    controlData.Throttle  = mapJoystickValues(analogRead(THROTTLE_PIN), 0, 511, 1023, false) + CURRENT_RX_CONFIG.ThrottleTrim * RX_TRIM_STEP_WIDTH;
    controlData.Yaw       = mapJoystickValues(analogRead(YAW_PIN), 0, 511, 1023, false) + CURRENT_RX_CONFIG.YawTrim * RX_TRIM_STEP_WIDTH;
    controlData.Pitch     = mapJoystickValues(analogRead(PITCH_PIN), 0, 511, 1023, false) + CURRENT_RX_CONFIG.PitchTrim * RX_TRIM_STEP_WIDTH;
    controlData.Roll      = mapJoystickValues(analogRead(ROLL_PIN), 0, 511, 1023, false) + CURRENT_RX_CONFIG.RollTrim * RX_TRIM_STEP_WIDTH;
    if (AUX_1_STATES == 0) controlData.Aux1 = mapJoystickValues(analogRead(AUX1_PIN), 0, 511, 1023, false);
    else if (AUX_1_STATES == 1) controlData.Aux1 = map(digitalRead(AUX1_PIN), 0, 1, 1000, 2000);
    else controlData.Aux1 = map(aux1State, 0, AUX_1_STATES - 1, 1000, 2000);
    if (AUX_2_STATES == 0) controlData.Aux2 = mapJoystickValues(analogRead(AUX2_PIN), 0, 511, 1023, false);
    else if (AUX_2_STATES == 1) controlData.Aux2 = map(digitalRead(AUX2_PIN), 0, 1, 1000, 2000);
    else controlData.Aux2 = map(aux2State, 0, AUX_2_STATES - 1, 1000, 2000);
    if (AUX_3_STATES == 0) controlData.Aux3 = mapJoystickValues(analogRead(AUX3_PIN), 0, 511, 1023, false);
    else if (AUX_3_STATES == 1) controlData.Aux3 = map(digitalRead(AUX3_PIN), 0, 1, 1000, 2000);
    else controlData.Aux3 = map(aux3State, 0, AUX_3_STATES - 1, 1000, 2000);
    if (AUX_4_STATES == 0) controlData.Aux4 = mapJoystickValues(analogRead(AUX4_PIN), 0, 511, 1023, false);
    else if (AUX_4_STATES == 1) controlData.Aux4 = map(digitalRead(AUX4_PIN), 0, 1, 1000, 2000);
    else controlData.Aux4 = map(aux4State, 0, AUX_4_STATES - 1, 1000, 2000);
#ifdef EXTEND_CHANNEL
    if (AUX_5_STATES == 0) controlData.Aux5 = mapJoystickValues(analogRead(AUX5_PIN), 0, 511, 1023, false);
    else if (AUX_5_STATES == 1) controlData.Aux5 = map(digitalRead(AUX5_PIN), 0, 1, 1000, 2000);
    else controlData.Aux5 = map(aux5State, 0, AUX_5_STATES - 1, 1000, 2000);
    if (AUX_6_STATES == 0) controlData.Aux6 = mapJoystickValues(analogRead(AUX6_PIN), 0, 511, 1023, false);
    else if (AUX_6_STATES == 1) controlData.Aux6 = map(digitalRead(AUX6_PIN), 0, 1, 1000, 2000);
    else controlData.Aux6 = map(aux6State, 0, AUX_6_STATES - 1, 1000, 2000);
#endif
#ifdef SWD_1
    controlData.Swd1      = mapJoystickValues(analogRead(SWD1_PIN), 0, 511, 1023, false);
#endif
#ifdef SWD_2
    controlData.Swd2      = mapJoystickValues(analogRead(SWD2_PIN), 0, 511, 1023, false);
#endif
#ifdef SHOW_RATE
    dataGetCount++;
#endif
  }
}

uint16_t mapJoystickValues(uint16_t val, uint16_t lower, uint16_t middle, uint16_t upper, bool reverse) {
  val = constrain(val, lower, upper);
  if (val < middle) val = map(val, lower, middle, 1000, 1500);
  else val = map(val, middle, upper, 1500, 2000);
  return reverse ? 3000 - val : val;
}

void trimStick(bool increase) {
  int8_t intendTrimValue;
  switch (trimmingStickIndex) {
    case 0: //YAW
      intendTrimValue = increase ? CURRENT_RX_CONFIG.YawTrim + RX_TRIM_STEP_WIDTH : CURRENT_RX_CONFIG.YawTrim - RX_TRIM_STEP_WIDTH;
      if (intendTrimValue > RX_TRIM_STEP_RANGE) {
        CURRENT_RX_CONFIG.YawTrim = RX_TRIM_STEP_RANGE;
      } else if (intendTrimValue < -RX_TRIM_STEP_RANGE) {
        CURRENT_RX_CONFIG.YawTrim = -RX_TRIM_STEP_RANGE;
      } else {
        CURRENT_RX_CONFIG.YawTrim = intendTrimValue;
      }
      break;
    case 1: //THR
      intendTrimValue = increase ? CURRENT_RX_CONFIG.ThrottleTrim + RX_TRIM_STEP_WIDTH : CURRENT_RX_CONFIG.ThrottleTrim - RX_TRIM_STEP_WIDTH;
      if (intendTrimValue > RX_TRIM_STEP_RANGE) {
        CURRENT_RX_CONFIG.ThrottleTrim = RX_TRIM_STEP_RANGE;
      } else if (intendTrimValue < -RX_TRIM_STEP_RANGE) {
        CURRENT_RX_CONFIG.ThrottleTrim = -RX_TRIM_STEP_RANGE;
      } else {
        CURRENT_RX_CONFIG.ThrottleTrim = intendTrimValue;
      }
      break;
    case 2: //PIT
      intendTrimValue = increase ? CURRENT_RX_CONFIG.PitchTrim + RX_TRIM_STEP_WIDTH : CURRENT_RX_CONFIG.PitchTrim - RX_TRIM_STEP_WIDTH;
      if (intendTrimValue > RX_TRIM_STEP_RANGE) {
        CURRENT_RX_CONFIG.PitchTrim = RX_TRIM_STEP_RANGE;
      } else if (intendTrimValue < -RX_TRIM_STEP_RANGE) {
        CURRENT_RX_CONFIG.PitchTrim = -RX_TRIM_STEP_RANGE;
      } else {
        CURRENT_RX_CONFIG.PitchTrim = intendTrimValue;
      }
      break;
    case 3: //ROL
      intendTrimValue = increase ? CURRENT_RX_CONFIG.RollTrim + RX_TRIM_STEP_WIDTH : CURRENT_RX_CONFIG.RollTrim - RX_TRIM_STEP_WIDTH;
      if (intendTrimValue > RX_TRIM_STEP_RANGE) {
        CURRENT_RX_CONFIG.RollTrim = RX_TRIM_STEP_RANGE;
      } else if (intendTrimValue < -RX_TRIM_STEP_RANGE) {
        CURRENT_RX_CONFIG.RollTrim = -RX_TRIM_STEP_RANGE;
      } else {
        CURRENT_RX_CONFIG.RollTrim = intendTrimValue;
      }
      break;
  }
  EEPROM_writeRxConfig(CURRENT_RX_CONFIG);
}
