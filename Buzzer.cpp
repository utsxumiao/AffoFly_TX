#include "Arduino.h"
#include "types.h"
#include "config.h"
#include "def.h"
#include "Buzzer.h"

#ifdef BUZZER
uint32_t buzzerBeepPattern[6] = {0, 0, 0, 0, 0, 0};
uint8_t buzzerBeepRepeats = 0;
uint8_t buzzerBeepPatternIndex = 0;
uint32_t beepingTime = 0;

void Buzzer_init(BuzzerBeepPattern pattern) {
  switch (pattern) {
    case buttonPress:
      setBuzzerValues(50000, 0, 0, 0, 0, 0, 1);
      break;
    case lowBattery:
      setBuzzerValues(1000000, 1000000, 0, 0, 0, 0, 10);
      break;
    case throttleWarning:
      setBuzzerValues(100000, 100000, 0, 0, 0, 0, 100);
      break;
    case radioBinding:
      setBuzzerValues(50000, 50000, 50000, 50000, 50000, 1000000, 100);
      break;
    default:
      setBuzzerValues(0, 0, 0, 0, 0, 0, 0);
      break;
  }
}

void setBuzzerValues(uint32_t firstBeepDuration, uint32_t firstPauseDuration, uint32_t secondBeepDuration, uint32_t secondPauseDuration, uint32_t thirdBeepDuration, uint32_t thirdPauseDuration, uint8_t repeats) {
  Serial.println("setting pattern...");
  buzzerBeepPattern[0] = firstBeepDuration;
  buzzerBeepPattern[1] = firstPauseDuration;
  buzzerBeepPattern[2] = secondBeepDuration;
  buzzerBeepPattern[3] = secondPauseDuration;
  buzzerBeepPattern[4] = thirdBeepDuration;
  buzzerBeepPattern[5] = thirdPauseDuration;
  buzzerBeepRepeats = repeats;
  buzzerBeepPatternIndex = 0;
}

void Buzzer_beep(uint32_t currentTime) {
  if (buzzerBeepRepeats) {
    if (buzzerBeepPatternIndex == 0) {
      beepingTime = currentTime;
    }

    if (currentTime >= beepingTime) {
      if (buzzerBeepPattern[buzzerBeepPatternIndex] == 0) {
        digitalWrite(BUZZER_PIN, LOW);
        Serial.print(currentTime); Serial.print("    ");
        Serial.print("repeats: "); Serial.print(buzzerBeepRepeats); Serial.print("    ");
        Serial.print("index: ");  Serial.print(buzzerBeepPatternIndex); Serial.print("    ");
        Serial.print("buzzer state: "); Serial.println(digitalRead(BUZZER_PIN));
        buzzerBeepPatternIndex = 0;
        buzzerBeepRepeats--;
      } else {
        digitalWrite(BUZZER_PIN, buzzerBeepPatternIndex % 2 == 0 ? HIGH : LOW);
        Serial.print(currentTime); Serial.print("    ");
        Serial.print("repeats: "); Serial.print(buzzerBeepRepeats); Serial.print("    ");
        Serial.print("index: ");  Serial.print(buzzerBeepPatternIndex); Serial.print("    ");
        Serial.print("buzzer state: "); Serial.println(digitalRead(BUZZER_PIN));
        beepingTime += buzzerBeepPattern[buzzerBeepPatternIndex];
        buzzerBeepPatternIndex++;
        if (buzzerBeepPatternIndex > 5) {
          buzzerBeepPatternIndex = 0;
          buzzerBeepRepeats--;
        }
      }
    }
  }
}


#endif
