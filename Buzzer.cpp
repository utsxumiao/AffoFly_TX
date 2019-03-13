#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Buzzer.h"

#ifdef BUZZER
uint32_t buzzerBeepPattern[6] = {0, 0, 0, 0, 0, 0};
uint8_t buzzerBeepRepeat = 0;
bool shouldRefresh = false;

uint8_t buzzerBeepPatternIndex = 0;
uint32_t previousBeepTime = 0;

void Buzzer_beep(uint32_t currentTime) {
  if (shouldRefresh)  {
    buzzerBeepPatternIndex = 0;
    shouldRefresh = false;
  }
  if (buzzerBeepRepeat) {
    if (buzzerBeepPattern[buzzerBeepPatternIndex]) {
      if (buzzerBeepPatternIndex == 0) {
        digitalWrite(BUZZER_PIN, HIGH);
      }
      digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
    }
    buzzerBeepPatternIndex++;
    if (buzzerBeepPatternIndex > 5) {
      buzzerBeepPatternIndex = 0;
      buzzerBeepRepeat--;
    }
  }
}
#endif
