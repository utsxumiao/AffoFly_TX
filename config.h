#ifndef CONFIG_H_
#define CONFIG_H_

/* ============================== DESCRIPTION ============================== */
/************************************************************************************/
/* 3-position switch is not supported
/* Pin Mapping */
/************************************************************************************/
/* Feel free to move around following pin if you know what you are doing.
   Becareful PIN A6 and A7 are analog input only, no internal pullup.

  THROTTLE   A0
  YAW        A1
  PITCH      A2
  ROLL       A3
  I2C        A4
  I2C        A5
  SWD_1      A6  WHEN SWD_1 is enabled
  SWD_2      A7==WHEN SWD_2 is enabled
  V_BAT      A7==WHEN V_BAT is enabled
  AUX_1      2
  AUX_2      3
  AUX_3      4
  AUX_4      5
  AUX_5      6===WHEN EXTEND_CHANNEL is enabled
  CPPM       6===WHEN SIMULATOR is enabled
  NRF_CE     7
  NRF_CSN    8
  AUX_6      9===WHEN EXTEND_CHANNEL is enabled
  BUZZER     9===WHEN BUZZER is enabled
  MENU       10
  SPI        11
  SPI        12
  SPI        13
*/


  /* BASIC SETUP */
/************************************************************************************/
/* Only one function can be enabled in each group as they share same pin.
   EXTEND_CHANNEL / BUZZER & SIMULATOR
   SWD_2 / V_BATTERY
*/
#define BUTTON_COUNT  5           // how many BUTTON type controls in your TX hardware

//#define EXTEND_CHANNEL          // Extends 2 extra AUX channels (AUX5 & AUX6)
#define BUZZER
#define SIMULATOR
#define BUDDY                   // Trainer & Student function

#define SWD_1

//#define SWD_2
#define V_BAT
#define V_BAT_ALARM_VOLTAGE 350    // voltage * 100;

/* AUX States:
     - 0:      SWD (Dail) type
     - 1:      Two position switch
     - n > 1:  Button type with n states (each press goes to next state and loop)
*/
#define AUX_1_STATES  2
#define AUX_2_STATES  3
#define AUX_3_STATES  2
#define AUX_4_STATES  2

#ifdef EXTEND_CHANNEL
#define AUX_5_STATES  2
#define AUX_6_STATES  2
#endif


/* DISPLAY SETUP */
/************************************************************************************/
//#define SCREEN_LCD_1602A_HW_I2C
//#define SCREEN_LCD_SSD1306_128X64_HW_I2C
#define SCREEN_SERIAL_MONITOR

//#define SHOW_WELCOME_SCREEN
#define SHOW_RATE
//anything related to display



/* RADIO SETUP */
/************************************************************************************/
#define NRF24
//other radio systems

#endif /* CONFIG_H_ */
