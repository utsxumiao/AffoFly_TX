#ifndef DEF_H_
#define DEF_H_

/**************************************************************************************/
/***************              Pro Mini Pin Definitions             ********************/
/**************************************************************************************/
#define THROTTLE_PIN      A0
#define YAW_PIN           A1
#define PITCH_PIN         A2
#define ROLL_PIN          A3

#ifdef SWD_1
#define SWD1_PIN          A6
#endif

#ifdef SWD_2
#define SWD2_PIN          A7
#endif

#ifdef V_BAT
#define V_BAT_PIN         A7
#endif

#define AUX1_PIN          2
#define AUX2_PIN          3
#define AUX3_PIN          4
#define AUX4_PIN          5

#if defined(SIMULATOR) || defined(BUDDY)
// CPPM.cpp also needs to be updated as we use pin register there for performance
#define CPPM_PIN          6   
#endif

#ifdef NRF24
#define NRF_CE_PIN        7
#define NRF_CSN_PIN       8
#endif

#ifdef BUZZER
#define BUZZER_PIN        9
#endif

#ifdef EXTEND_CHANNEL
#define AUX5_PIN          6
#define AUX6_PIN          9
#endif

#define FN_PIN            10

#define BUTTON_BACK_PIN   2
#define BUTTON_GO_PIN     3
#define BUTTON_PREV_PIN   4
#define BUTTON_NEXT_PIN   5




/**************************************************************************************/
/***************             General Configurations                ********************/
/**************************************************************************************/
//#define DEBUG
#define PROJECT_NAME        "AffoFly TX"
#define PROJECT_VERSION     "v0.020"

#define MODE_CONTROL        0
#define MODE_SIMULATOR      1
#define MODE_TRAINER        2
#define MODE_STUDENT        3
#define MODE_SETTING        4
#define MODE_MENU           5

#define MENU_TITLE_LENGTH   15
#define MENU_ITEM_LENGTH    15

// EEPROM address 0-99 for general TX settings
// Each RxConfig takes 20 bytes (current data use 16 bytes)
// We allocate 10 RxConfig
// EEPROM address 100-299 for 10 RxConfig
#define RADIO_UNIQUE_ID_EEPROM_ADDRESS  0
#define RADIO_UNIQUE_ID_LOWER_BOUNDARY  1000
#define RADIO_UNIQUE_ID_UPPER_BOUNDARY  9999

#define RADIO_PA_LEVEL_EEPROM_ADDRESS   4
#define RADIO_PA_LEVEL_LOWER_BOUNDARY   0
#define RADIO_PA_LEVEL_UPPER_BOUNDARY   3

#define CURRENT_RX_ID_EEPROM_ADDRESS    5

#define RX_CONFIG_EEPROM_START_ADDRESS  100
#define RX_CONFIG_ALLOCATED_BYTES       20
#define RADIO_RX_ID_LOWER_BOUNDARY      1
#define RADIO_RX_ID_UPPER_BOUNDARY      10
#define RADIO_CHANNEL_LOWER_BOUNDARY    100
#define RADIO_CHANNEL_UPPER_BOUNDARY    125
#define RX_TRIM_STEP_RANGE              15
#define RX_TRIM_STEP_WIDTH              2

// CPPM Configurations, 
#define CLOCK_MULTIPLIER  1       // 1 for 8MHZ, 2 for 16MHZ
#define PPM_FRAME_LENGTH  20000
#define PPM_PULSE_LENGTH  400
#define CHANNEL_COUNT     12

#endif /* DEF_H_ */
