#ifndef BUZZER_H_
#define BUZZER_H_

#ifdef BUZZER
extern uint16_t buzzerBeepPattern[6]; // firstBeepDuration, firstPauseDuration, secondBeepDuration, secondPauseDuration, thirdBeepDuration, cyclePauseDuration
extern uint8_t buzzerBeepRepeat;
extern uint8_t buzzerBeepPatternIndex;

#endif

#endif /* BUZZER_H_ */
