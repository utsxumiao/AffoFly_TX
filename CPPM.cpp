#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Bounce2.h"
#include "AffoFly_Transmitter.h"
#include "CPPM.h"

#if defined(SIMULATOR) || defined(BUDDY)
#ifdef SHOW_RATE
uint16_t CPPM_COUNT = 0;
#endif
uint16_t PPM[CHANNEL_COUNT];

void CPPM_init() {
  pinMode(CPPM_PIN, OUTPUT);
  PORTD = PORTD & ~B01000000;  //Set CPPM to low
  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;
  OCR1A = 100;  // compare match register (not very important, sets the timeout for the first interrupt)
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();
}

void CPPM_outputData(ControlData controlData) {
  PPM[0] = controlData.Throttle;
  PPM[1] = controlData.Yaw;
  PPM[2] = controlData.Pitch;
  PPM[3] = controlData.Roll;
  PPM[4] = controlData.Aux1;
  PPM[5] = controlData.Aux2;
  PPM[6] = controlData.Aux3;
  PPM[7] = controlData.Aux4;
  PPM[8] = controlData.Aux5;
  PPM[9] = controlData.Aux6;
  PPM[10] = controlData.Swd1;
#ifdef SHOW_RATE
  CPPM_COUNT++;
#endif
}

ISR(TIMER1_COMPA_vect) {
  if (TX_MODE != MODE_SIMULATOR && TX_MODE != MODE_TRAINER) return; //TODO: trainer/student
  static boolean state = true;
  TCNT1 = 0;
  if ( state ) {
    //end pulse
    PORTD = PORTD & ~B01000000; // turn pin 6 off.
    OCR1A = PPM_PULSE_LENGTH * CLOCK_MULTIPLIER;
    state = false;
  } else {
    //start pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;

    PORTD = PORTD | B01000000; // turn pin 6 on.
    state = true;

    if (cur_chan_numb >= CHANNEL_COUNT) {
      cur_chan_numb = 0;
      calc_rest += PPM_PULSE_LENGTH;
      OCR1A = (PPM_FRAME_LENGTH - calc_rest) * CLOCK_MULTIPLIER;
      calc_rest = 0;
    }
    else {
      OCR1A = (PPM[cur_chan_numb] - PPM_PULSE_LENGTH) * CLOCK_MULTIPLIER;
      calc_rest += PPM[cur_chan_numb];
      cur_chan_numb++;
    }
  }
}

#endif
