#include "pitches.h"

// Happy Birthday melody:
int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,    // Happy Birthday to You
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,    // Happy Birthday to You
  NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4, // Happy Birthday Dear [Name]
  NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4   // Happy Birthday to You
};

// Note durations: 4 = quarter note, 8 = eighth note, etc.
int noteDurations[] = {
  8, 8, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 2
};

void setup() {
  for (int thisNote = 0; thisNote < 26; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];  
    tone(BDPIN_BUZZER, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BDPIN_BUZZER);
  }
}

void loop() {
  // No repeat
}
