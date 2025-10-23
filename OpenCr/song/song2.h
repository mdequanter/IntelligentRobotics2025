#include "pitches.h"

int BDPIN_BUZZER = 9;


// notes in the melody:
// Notes for "Happy Birthday"
int HappyBirthday[] = {
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,  // Happy Birthday to You
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,  // Happy Birthday to You
  NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4, // Happy Birthday dear [name]
  NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4  // Happy Birthday to You
};

// Note durations: 4 = quarter note, 8 = eighth note, etc.
int noteDurations_HappyBir  thday[] = {
  8, 8, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 4, 2,
  8, 8, 4, 4, 4, 2
};


void playSequence(int melody[], int noteDurations[]){
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BDPIN_BUZZER, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(BDPIN_BUZZER);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Playing notes");

}

void loop() {
  delay(1000);
  Serial.println("Start playing : Happy Birthday");
  playSequence(HappyBirthday,noteDurations_HappyBirthday);
}
