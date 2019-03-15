#ifndef BUZZER_H_
#define BUZZER_H_

#ifdef BUZZER

void Buzzer_start(BuzzerBeepPattern pattern);
void Buzzer_beep(uint32_t currentTime);

void setBuzzerValues(uint32_t firstBeepDuration, uint32_t firstPauseDuration, uint32_t secondBeepDuration, uint32_t secondPauseDuration, uint32_t thirdBeepDuration, uint32_t thirdPauseDuration, uint8_t repeats);

#endif

#endif /* BUZZER_H_ */
