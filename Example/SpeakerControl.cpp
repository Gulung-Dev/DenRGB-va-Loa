#include "SpeakerControl.h" 
#include <Arduino.h>

SpeakerControl::SpeakerControl(int speakerPin, int defaultVolume) 
  : _speakerPin(speakerPin), _currentVolume(defaultVolume) { 
}

void SpeakerControl::begin() {
    pinMode(_speakerPin, OUTPUT);
    Serial.println("SpeakerControl da khoi tao.");
}

void SpeakerControl::play() { 
    Serial.println("Bat dau phat nhac...");
    
    int melody[] = {
        NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4,
        NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4,
        NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4,
        NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4,
        NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4,
        NOTE_D5, NOTE_D5, NOTE_D5, NOTE_D5,
        NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, 
        NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, 
        NOTE_G5, NOTE_G5, NOTE_G5, NOTE_G5,
        NOTE_G5, NOTE_G5, NOTE_G5, NOTE_G5, 
        NOTE_G5, NOTE_G5, NOTE_G5, NOTE_G5, 
        NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_F4,
        NOTE_G4, 0, NOTE_G4, NOTE_D5,
        NOTE_C5, 0, NOTE_AS4, 0,
        NOTE_A4, 0, NOTE_A4, NOTE_A4,
        NOTE_C5, 0, NOTE_AS4, NOTE_A4, 
        NOTE_G4,0, NOTE_G4, NOTE_AS5,
        NOTE_A5, NOTE_AS5, NOTE_A5, NOTE_AS5,
        NOTE_G4,0, NOTE_G4, NOTE_AS5,
        NOTE_A5, NOTE_AS5, NOTE_A5, NOTE_AS5,
        NOTE_G4, 0, NOTE_G4, NOTE_D5,
        NOTE_C5, 0, NOTE_AS4, 0,
        NOTE_A4, 0, NOTE_A4, NOTE_A4,
        NOTE_C5, 0, NOTE_AS4, NOTE_A4, 
        NOTE_G4,0, NOTE_G4, NOTE_AS5,
        NOTE_A5, NOTE_AS5, NOTE_A5, NOTE_AS5,
        NOTE_G4,0, NOTE_G4, NOTE_AS5,
        NOTE_A5, NOTE_AS5, NOTE_A5, NOTE_AS5
    };

    int melodySize = sizeof(melody) / sizeof(melody[0]);

    for (int i = 0; i < melodySize; i += 2) {
        int note = melody[i];
        int duration = melody[i + 1];

        if (note > 0) {
            tone(_speakerPin, note); 
        } else {
            noTone(_speakerPin);
        }
        delay(duration);
    }
    noTone(_speakerPin);
    Serial.println("Ket thuc phat nhac.");
}

void SpeakerControl::stopPlaying() { 
    noTone(_speakerPin);
    Serial.println("Da dung phat nhac.");
}

int SpeakerControl::getVolume() const { 
    return _currentVolume;
}

void SpeakerControl::setVolume(int volume) {
    _currentVolume = constrain(volume, 0, 100);
    Serial.print("Am luong duoc dat: ");
    Serial.println(_currentVolume);
}