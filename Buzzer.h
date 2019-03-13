#ifndef BUZZER_H_
#define BUZZER_H_

#ifdef BUZZER
extern uint32_t buzzerBeepPattern[6]; // firstBeepDuration, firstPauseDuration, secondBeepDuration, secondPauseDuration, thirdBeepDuration, cyclePauseDuration
extern uint8_t buzzerBeepRepeat;
extern bool shouldRefresh;

void Buzzer_beep(uint32_t currentTime);

#endif

#endif /* BUZZER_H_ */
