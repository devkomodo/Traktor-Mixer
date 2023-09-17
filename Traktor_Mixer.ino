#include <MIDI.h>
#include <Encoder.h>

const int faderPin = A1;    // Pin analog untuk fader
const int deckAPin = A0;    // Pin analog untuk deck A
const int deckBPin = A0;    // Pin analog untuk deck B

const int EQ_High_A = A0;    // Pin analog untuk EQ High A
const int EQ_Medium_A = A0;    // Pin analog untuk EQ Medium
const int EQ_Low_A = A3;    // Pin analog untuk EQ Low A

const int EQ_High_B = A0;    // Pin analog untuk EQ High B
const int EQ_Medium_B = A0;    // Pin analog untuk EQ Medium B
const int EQ_Low_B = A2;    // Pin analog untuk EQ Low B

int lastEQHighAValue = 0;
int lastEQMediumAValue = 0;
int lastEQLowAValue = 0;

int lastEQHighBValue = 0;
int lastEQMediumBValue = 0;
int lastEQLowBValue = 0;

const int syncAPin = 42;    // Pin digital untuk tombol SYNC A
const int syncBPin = 40;    // Pin digital untuk tombol SYNC B

const int playAPin = 46;    // Pin digital untuk tombol PLAY A
const int playBPin = 44;    // Pin digital untuk tombol PLAY B

const int FXAPin = 2;    // Pin digital untuk tombol FX A
const int FXBPin = 3;    // Pin digital untuk tombol FX B

const int ledPinFXA = 4;  // Ganti dengan pin yang sesuai untuk LED FX Deck A
const int ledPinFXB = 4;  // Ganti dengan pin yang sesuai untuk LED FX Deck B

boolean previousFXAState = false;
boolean previousFXBState = false;
unsigned long previousMillisFXA = 0;
unsigned long previousMillisFXB = 0;
const long interval = 500;  // Waktu inte

const int CFXPin = 30;    // Pin digital untuk tombol CFX ON
const int CFXCH1Pin = A11;    // Pin digital untuk potensiometer CFX CH1
const int CFXCH2Pin = A12;    // Pin digital untuk potensiometer CFX CH2

const int CUE1APin = 20;    // Pin digital untuk tombol CUE1 deck A
const int CUE2APin = 21;    // Pin digital untuk tombol CUE2 deck A
const int CUE3APin = 22;    // Pin digital untuk tombol CUE3 deck A
const int CUE4APin = 23;    // Pin digital untuk tombol CUE4 deck A

const int CUE1BPin = 10;    // Pin digital untuk tombol CUE1 deck B
const int CUE2BPin = 11;    // Pin digital untuk tombol CUE2 deck B
const int CUE3BPin = 12;    // Pin digital untuk tombol CUE3 deck B
const int CUE4BPin = 13;    // Pin digital untuk tombol CUE4 deck B

// Headphone Cue
const int CUEDeckAPin = 14;    // Pin digital untuk tombol Cue Monitor Deck A
const int CUEDeckBPin = 15;    // Pin digital untuk tombol Cue Monitor Deck B

const int numSamples = 10;   // Jumlah sampel yang diambil untuk smoothing
const int debounceDelay = 50; // Waktu debounce dalam milidetik

int smoothedValueFader = 0;
int smoothedValueDeckA = 0;
int smoothedValueDeckB = 0;

boolean syncAState = false;
boolean syncBState = false;
boolean playAState = false;
boolean playBState = false;

boolean cueDeckAState = false;
boolean cueDeckBState = false;

boolean FXAState = false;
boolean FXBState = false;
boolean CFXState = false;
int prevCFXCH1Value = -1;
int prevCFXCH2Value = -1;
boolean CUE1AState = false;
boolean CUE2AState = false;
boolean CUE3AState = false;
boolean CUE4AState = false;
boolean CUE1BState = false;
boolean CUE2BState = false;
boolean CUE3BState = false;
boolean CUE4BState = false;

unsigned long lastSyncATime = 0;
unsigned long lastSyncBTime = 0;
unsigned long lastPlayATime = 0;
unsigned long lastPlayBTime = 0;
unsigned long lastFXATime = 0;
unsigned long lastFXBTime = 0;
unsigned long lastCFXTime = 0;
unsigned long lastCUE1ATime = 0;
unsigned long lastCUE2ATime = 0;
unsigned long lastCUE3ATime = 0;
unsigned long lastCUE4ATime = 0;
unsigned long lastCUE1BTime = 0;
unsigned long lastCUE2BTime = 0;
unsigned long lastCUE3BTime = 0;
unsigned long lastCUE4BTime = 0;
unsigned long lastCueDeckATime = 0;
unsigned long lastCueDeckBTime = 0;

int prevMidiValueFader = -1;
int prevMidiValueDeckA = -1;
int prevMidiValueDeckB = -1;


MIDI_CREATE_DEFAULT_INSTANCE();
Encoder browseEncoder(2, 3);  // Inisialisasi encoder dengan pin 2 (DT) dan pin 3 (CLK)

void setup() {

  pinMode(syncAPin, INPUT_PULLUP);
  pinMode(syncBPin, INPUT_PULLUP);
  pinMode(playAPin, INPUT_PULLUP);
  pinMode(playBPin, INPUT_PULLUP);
  pinMode(ledPinFXA, OUTPUT);
  pinMode(ledPinFXB, OUTPUT);
  // Inisialisasi pin-pin untuk tombol Cue Monitor Deck A dan Deck B
  pinMode(CUEDeckAPin, INPUT_PULLUP);
  pinMode(CUEDeckBPin, INPUT_PULLUP);
  MIDI.begin();
  Serial.begin(115200); // Komunikasi Hairless MIDI
}

void loop() {
  smoothedValueFader = smoothAnalogValue(faderPin);
  smoothedValueDeckA = smoothAnalogValue(deckAPin);
  smoothedValueDeckB = smoothAnalogValue(deckBPin);

  int midiValueFader = map(smoothedValueFader, 0, 1023, 0, 127);
  int midiValueDeckA = map(smoothedValueDeckA, 0, 1023, 0, 127);
  int midiValueDeckB = map(smoothedValueDeckB, 0, 1023, 0, 127);

  // Baca status tombol SYNC A, SYNC B, PLAY A, PLAY B, FX A, FX B, CFX ON, CFX CH1, CFX CH2, CUE1-4 A, CUE1-4 B,Cue Monitor A dan B
  // Lalu, tambahkan logika penanganan sesuai kebutuhan Anda
  boolean newSyncAState = digitalRead(syncAPin);
  boolean newSyncBState = digitalRead(syncBPin);
  boolean newPlayAState = digitalRead(playAPin);
  boolean newPlayBState = digitalRead(playBPin);
  boolean newFXAState = digitalRead(FXAPin);
  boolean newFXBState = digitalRead(FXBPin);


  if (midiValueFader != prevMidiValueFader) {
    MIDI.sendControlChange(1, midiValueFader, 1);  // Ch 1 CC 001 (Fader)
    prevMidiValueFader = midiValueFader;
  }
  //
  //  if (midiValueDeckA != prevMidiValueDeckA) {
  //    MIDI.sendControlChange(2, midiValueDeckA, 1);  // Ch 1 CC 002 (Deck A)
  //    prevMidiValueDeckA = midiValueDeckA;
  //  }
  //
  //  if (midiValueDeckB != prevMidiValueDeckB) {
  //    MIDI.sendControlChange(3, midiValueDeckB, 1);  // Ch 1 CC 003 (Deck B)
  //    prevMidiValueDeckB = midiValueDeckB;
  //  }
  //
  //  // Handle tombol SYNC A
  //  if (newSyncAState != syncAState) {
  //    if (millis() - lastSyncATime >= debounceDelay) {
  //      syncAState = newSyncAState;
  //      sendMIDINoteOnOff(3, 1, syncAState);  // Ch 4 CC 005 (SYNC A)
  //      lastSyncATime = millis();
  //    }
  //  }
  //
  //  // Handle tombol SYNC B
  //  if (newSyncBState != syncBState) {
  //    if (millis() - lastSyncBTime >= debounceDelay) {
  //      syncBState = newSyncBState;
  //      sendMIDINoteOnOff(3, 2, syncBState);  // Ch 4 CC 002 (SYNC B)
  //      lastSyncBTime = millis();
  //    }
  //  }
  //
  //  // Handle tombol PLAY A
  //  if (newPlayAState != playAState) {
  //    if (millis() - lastPlayATime >= debounceDelay) {
  //      playAState = newPlayAState;
  //      sendMIDINoteOnOff(3, 3, playAState);  // Ch 4 CC 003 (PLAY A)
  //      lastPlayATime = millis();
  //    }
  //  }
  //
  //  // Handle tombol PLAY B
  //  if (newPlayBState != playBState) {
  //    if (millis() - lastPlayBTime >= debounceDelay) {
  //      playBState = newPlayBState;
  //      sendMIDINoteOnOff(3, 4, playBState);  // Ch 4 CC 004(PLAY B)
  //      lastPlayBTime = millis();
  //    }
  //  }

  //  // Handle tombol FX A
  //  if (newFXAState != FXAState) {
  //    if (millis() - lastFXATime >= debounceDelay) {
  //      FXAState = newFXAState;
  //      if (FXAState) {
  //        // Tombol FX A ditekan, kirim pesan MIDI untuk mengaktifkan efek
  //        MIDI.sendControlChange(4, 1, 1);  // Ch 4 CC 001 (Aktifkan FX A)
  //      } else {
  //        // Tombol FX A dilepas, kirim pesan MIDI untuk menonaktifkan efek
  //        MIDI.sendControlChange(4, 1, 0);  // Ch 4 CC 001 (Nonaktifkan FX A)
  //      }
  //      lastFXATime = millis();
  //    }
  //  }
  //
  //  // Handle tombol FX B
  //  if (newFXBState != FXBState) {
  //    if (millis() - lastFXBTime >= debounceDelay) {
  //      FXBState = newFXBState;
  //      if (FXBState) {
  //        // Tombol FX B ditekan, kirim pesan MIDI untuk mengaktifkan efek
  //        MIDI.sendControlChange(4, 2, 1);  // Ch 4 CC 002 (Aktifkan FX B)
  //      } else {
  //        // Tombol FX B dilepas, kirim pesan MIDI untuk menonaktifkan efek
  //        MIDI.sendControlChange(4, 2, 0);  // Ch 4 CC 002 (Nonaktifkan FX B)
  //      }
  //      lastFXBTime = millis();
  //    }
  //  }
  // Handle tombol FX A
  if (newFXAState != FXAState) {
    if (millis() - lastFXATime >= debounceDelay) {
      FXAState = newFXAState;
      if (FXAState) {
        // Tombol FX A ditekan, kirim pesan MIDI untuk mengaktifkan efek
        MIDI.sendControlChange(4, 1, 1);  // Ch 4 CC 001 (Aktifkan FX A)
      } else {
        // Tombol FX A dilepas, kirim pesan MIDI untuk menonaktifkan efek
        MIDI.sendControlChange(4, 1, 0);  // Ch 4 CC 001 (Nonaktifkan FX A)
        digitalWrite(ledPinFXA, LOW);   // Matikan LED FX Deck A

      }
      lastFXATime = millis();
    }
  }

  // Handle tombol FX B
  if (newFXBState != FXBState) {
    if (millis() - lastFXBTime >= debounceDelay) {
      FXBState = newFXBState;
      if (FXBState) {
        // Tombol FX B ditekan, kirim pesan MIDI untuk mengaktifkan efek
        MIDI.sendControlChange(4, 2, 1);  // Ch 4 CC 002 (Aktifkan FX B)
      } else {
        // Tombol FX B dilepas, kirim pesan MIDI untuk menonaktifkan efek
        MIDI.sendControlChange(4, 2, 0);  // Ch 4 CC 002 (Nonaktifkan FX B)
        digitalWrite(ledPinFXB, LOW);   // Matikan LED FX Deck B

      }
      lastFXBTime = millis();
    }
  }

  // Kedipkan LED FX A setiap 500ms
  unsigned long currentMillis = millis();
  if (FXAState && currentMillis - previousMillisFXA >= interval) {
    previousMillisFXA = currentMillis;
    // Ubah status LED FX A
    if (digitalRead(ledPinFXA) == LOW) {
      digitalWrite(ledPinFXA, HIGH);  // Nyalakan LED FX Deck A
    } else {
      digitalWrite(ledPinFXA, LOW);   // Matikan LED FX Deck A
    }
  }

  // Kedipkan LED FX B setiap 500ms
  if (FXBState && currentMillis - previousMillisFXB >= interval) {
    previousMillisFXB = currentMillis;
    // Ubah status LED FX B
    if (digitalRead(ledPinFXB) == LOW) {
      digitalWrite(ledPinFXB, HIGH);  // Nyalakan LED FX Deck B
    } else {
      digitalWrite(ledPinFXB, LOW);   // Matikan LED FX Deck B
    }
  }
  // Panggil fungsi EQControl() untuk mengirim pesan MIDI untuk kontrol EQ
  EQControl();
  //  // Handle CFX
  //  ColorFX();
  //  // Handle Cue 1-4 Deck A
  //  cueDeckA();
  //  // Handle Cue 1-4 Deck B
  //  cueDeckB();
  //  // Handle Cue Monitor
  //  CueMonitor();
  //
  //  // Handle encoder untuk browse lagu up & down (Midi Control)
  //  int browseEncoderValue = browseEncoder.read();
  //  if (browseEncoderValue > 0) {
  //    // Encoder di putar ke kanan, kirim pesan MIDI untuk browse lagu ke atas
  //    MIDI.sendControlChange(6, 1, 1);  // Ch 6 CC 001 (Browse Lagu Atas)
  //  } else if (browseEncoderValue < 0) {
  //    // Encoder di putar ke kiri, kirim pesan MIDI untuk browse lagu ke bawah
  //    MIDI.sendControlChange(6, 2, 1);  // Ch 6 CC 002 (Browse Lagu Bawah)
  //  }
}
void EQControl() {
  int newEQHighAValue = analogRead(EQ_High_A);
  int newEQMediumAValue = analogRead(EQ_Medium_A);
  int newEQLowAValue = analogRead(EQ_Low_A);

  int newEQHighBValue = analogRead(EQ_High_B);
  int newEQMediumBValue = analogRead(EQ_Medium_B);
  int newEQLowBValue = analogRead(EQ_Low_B);
  //
  //  if (newEQHighAValue != lastEQHighAValue) {
  //    // Nilai EQ High A berubah, kirim pesan MIDI sesuai kebutuhan Anda
  //    MIDI.sendControlChange(10, map(newEQHighAValue, 0, 1023, 0, 127), 1);  // Contoh: Ch 1, CC 10 (EQ High A)
  //    lastEQHighAValue = newEQHighAValue;
  //  }
  //
  //  if (newEQMediumAValue != lastEQMediumAValue) {
  //    // Nilai EQ Medium A berubah, kirim pesan MIDI sesuai kebutuhan Anda
  //    MIDI.sendControlChange(11, map(newEQMediumAValue, 0, 1023, 0, 127), 1);  // Contoh: Ch 1, CC 11 (EQ Medium A)
  //    lastEQMediumAValue = newEQMediumAValue;
  //  }

  if (newEQLowAValue != lastEQLowAValue) {
    // Nilai EQ Low A berubah, kirim pesan MIDI sesuai kebutuhan Anda
    MIDI.sendControlChange(12, map(newEQLowAValue, 0, 1023, 0, 127), 1);  // Contoh: Ch 1, CC 12 (EQ Low A)
    lastEQLowAValue = newEQLowAValue;
  }
  //
  //  if (newEQHighBValue != lastEQHighBValue) {
  //    // Nilai EQ High B berubah, kirim pesan MIDI sesuai kebutuhan Anda
  //    MIDI.sendControlChange(13, map(newEQHighBValue, 0, 1023, 0, 127), 2);  // Contoh: Ch 2, CC 13 (EQ High B)
  //    lastEQHighBValue = newEQHighBValue;
  //  }
  //
  //  if (newEQMediumBValue != lastEQMediumBValue) {
  //    // Nilai EQ Medium B berubah, kirim pesan MIDI sesuai kebutuhan Anda
  //    MIDI.sendControlChange(14, map(newEQMediumBValue, 0, 1023, 0, 127), 2);  // Contoh: Ch 2, CC 14 (EQ Medium B)
  //    lastEQMediumBValue = newEQMediumBValue;
  //  }

  if (newEQLowBValue != lastEQLowBValue) {
    // Nilai EQ Low B berubah, kirim pesan MIDI sesuai kebutuhan Anda
    MIDI.sendControlChange(15, map(newEQLowBValue, 0, 1023, 0, 127), 2);  // Contoh: Ch 2, CC 15 (EQ Low B)
    lastEQLowBValue = newEQLowBValue;
  }
}

void ColorFX() {
  boolean newCFXState = digitalRead(CFXPin);
  // Handle tombol CFX ON
  if (newCFXState != CFXState) {
    if (millis() - lastCFXTime >= debounceDelay) {
      CFXState = newCFXState;
      if (CFXState) {
        // Tombol CFX ON ditekan, kirim pesan MIDI untuk mengaktifkan CFX
        MIDI.sendControlChange(4, 3, 1);  // Ch 4 CC 003 (Aktifkan CFX)
      } else {
        // Tombol CFX ON dilepas, kirim pesan MIDI untuk menonaktifkan CFX
        MIDI.sendControlChange(4, 3, 0);  // Ch 4 CC 003 (Nonaktifkan CFX)
      }
      lastCFXTime = millis();
    }
  }

  // Handle parameter CFX A (CH 1)
  int cfxCH1Value = analogRead(CFXCH1Pin);
  if (cfxCH1Value != prevCFXCH1Value) {
    // Nilai potensiometer CFX CH1 berubah, kirim pesan MIDI untuk mengatur parameter CFX A (CH 1)
    MIDI.sendControlChange(5, cfxCH1Value >> 3, 1);  // Ch 5 CC 001 (Atur Parameter CFX A CH1)
    prevCFXCH1Value = cfxCH1Value;
  }

  // Handle parameter CFX B (CH 2)
  int cfxCH2Value = analogRead(CFXCH2Pin);
  if (cfxCH2Value != prevCFXCH2Value) {
    // Nilai potensiometer CFX CH2 berubah, kirim pesan MIDI untuk mengatur parameter CFX B (CH 2)
    MIDI.sendControlChange(5, cfxCH2Value >> 3, 2);  // Ch 5 CC 002 (Atur Parameter CFX B CH2)
    prevCFXCH2Value = cfxCH2Value;
  }
}

void cueDeckA() {

  boolean newCUE1AState = digitalRead(CUE1APin);
  boolean newCUE2AState = digitalRead(CUE2APin);
  boolean newCUE3AState = digitalRead(CUE3APin);
  boolean newCUE4AState = digitalRead(CUE4APin);
  // Handle Cue 1 Deck A
  if (newCUE1AState != CUE1AState) {
    if (millis() - lastCUE1ATime >= debounceDelay) {
      CUE1AState = newCUE1AState;
      if (CUE1AState) {
        // Tombol Cue 1 Deck A ditekan, kirim pesan MIDI untuk mengaktifkan Cue 1 Deck A
        MIDI.sendNoteOn(60, 127, 4);  // Ch 4 NoteOn (Cue 1 Deck A)
      } else {
        // Tombol Cue 1 Deck A dilepas, kirim pesan MIDI untuk menonaktifkan Cue 1 Deck A
        MIDI.sendNoteOff(60, 0, 4);  // Ch 4 NoteOff (Cue 1 Deck A)
      }
      lastCUE1ATime = millis();
    }
  }
  // Handle Cue 2 Deck A
  if (newCUE2AState != CUE2AState) {
    if (millis() - lastCUE2ATime >= debounceDelay) {
      CUE2AState = newCUE2AState;
      if (CUE2AState) {
        // Tombol Cue 2 Deck A ditekan, kirim pesan MIDI untuk mengaktifkan Cue 2 Deck A
        MIDI.sendNoteOn(61, 127, 4);  // Ch 4 NoteOn (Cue 2 Deck A)
      } else {
        // Tombol Cue 2 Deck A dilepas, kirim pesan MIDI untuk menonaktifkan Cue 2 Deck A
        MIDI.sendNoteOff(61, 0, 4);  // Ch 4 NoteOff (Cue 2 Deck A)
      }
      lastCUE2ATime = millis();
    }
  }

  // Handle Cue 3 Deck A
  if (newCUE3AState != CUE3AState) {
    if (millis() - lastCUE3ATime >= debounceDelay) {
      CUE3AState = newCUE3AState;
      if (CUE3AState) {
        // Tombol Cue 3 Deck A ditekan, kirim pesan MIDI untuk mengaktifkan Cue 3 Deck A
        MIDI.sendNoteOn(62, 127, 4);  // Ch 4 NoteOn (Cue 3 Deck A)
      } else {
        // Tombol Cue 3 Deck A dilepas, kirim pesan MIDI untuk menonaktifkan Cue 3 Deck A
        MIDI.sendNoteOff(62, 0, 4);  // Ch 4 NoteOff (Cue 3 Deck A)
      }
      lastCUE3ATime = millis();
    }
  }

  // Handle Cue 4 Deck A
  if (newCUE4AState != CUE4AState) {
    if (millis() - lastCUE4ATime >= debounceDelay) {
      CUE4AState = newCUE4AState;
      if (CUE4AState) {
        // Tombol Cue 4 Deck A ditekan, kirim pesan MIDI untuk mengaktifkan Cue 4 Deck A
        MIDI.sendNoteOn(63, 127, 4);  // Ch 4 NoteOn (Cue 4 Deck A)
      } else {
        // Tombol Cue 4 Deck A dilepas, kirim pesan MIDI untuk menonaktifkan Cue 4 Deck A
        MIDI.sendNoteOff(63, 0, 4);  // Ch 4 NoteOff (Cue 4 Deck A)
      }
      lastCUE4ATime = millis();
    }
  }
}

void cueDeckB() {

  boolean newCUE1BState = digitalRead(CUE1BPin);
  boolean newCUE2BState = digitalRead(CUE2BPin);
  boolean newCUE3BState = digitalRead(CUE3BPin);
  boolean newCUE4BState = digitalRead(CUE4BPin);
  // Handle Cue 1 Deck B
  if (newCUE1BState != CUE1BState) {
    if (millis() - lastCUE1BTime >= debounceDelay) {
      CUE1BState = newCUE1BState;
      if (CUE1BState) {
        // Tombol Cue 1 Deck B ditekan, kirim pesan MIDI untuk mengaktifkan Cue 1 Deck B
        MIDI.sendNoteOn(62, 127, 5);  // Ch 5 NoteOn (Cue 1 Deck B)
      } else {
        // Tombol Cue 1 Deck B dilepas, kirim pesan MIDI untuk menonaktifkan Cue 1 Deck B
        MIDI.sendNoteOff(62, 0, 5);  // Ch 5 NoteOff (Cue 1 Deck B)
      }
      lastCUE1BTime = millis();
    }
  }
  // Handle Cue 2 Deck B
  if (newCUE2BState != CUE2BState) {
    if (millis() - lastCUE2BTime >= debounceDelay) {
      CUE2BState = newCUE2BState;
      if (CUE2BState) {
        // Tombol Cue 2 Deck B ditekan, kirim pesan MIDI untuk mengaktifkan Cue 2 Deck B
        MIDI.sendNoteOn(63, 127, 5);  // Ch 5 NoteOn (Cue 2 Deck B)
      } else {
        // Tombol Cue 2 Deck B dilepas, kirim pesan MIDI untuk menonaktifkan Cue 2 Deck B
        MIDI.sendNoteOff(63, 0, 5);  // Ch 5 NoteOff (Cue 2 Deck B)
      }
      lastCUE2BTime = millis();
    }
  }

  // Handle Cue 3 Deck B
  if (newCUE3BState != CUE3BState) {
    if (millis() - lastCUE3BTime >= debounceDelay) {
      CUE3BState = newCUE3BState;
      if (CUE3BState) {
        // Tombol Cue 3 Deck B ditekan, kirim pesan MIDI untuk mengaktifkan Cue 3 Deck B
        MIDI.sendNoteOn(64, 127, 5);  // Ch 5 NoteOn (Cue 3 Deck B)
      } else {
        // Tombol Cue 3 Deck B dilepas, kirim pesan MIDI untuk menonaktifkan Cue 3 Deck B
        MIDI.sendNoteOff(64, 0, 5);  // Ch 5 NoteOff (Cue 3 Deck B)
      }
      lastCUE3BTime = millis();
    }
  }

  // Handle Cue 4 Deck B
  if (newCUE4BState != CUE4BState) {
    if (millis() - lastCUE4BTime >= debounceDelay) {
      CUE4BState = newCUE4BState;
      if (CUE4BState) {
        // Tombol Cue 4 Deck B ditekan, kirim pesan MIDI untuk mengaktifkan Cue 4 Deck B
        MIDI.sendNoteOn(65, 127, 5);  // Ch 5 NoteOn (Cue 4 Deck B)
      } else {
        // Tombol Cue 4 Deck B dilepas, kirim pesan MIDI untuk menonaktifkan Cue 4 Deck B
        MIDI.sendNoteOff(65, 0, 5);  // Ch 5 NoteOff (Cue 4 Deck B)
      }
      lastCUE4BTime = millis();
    }
  }
}


void CueMonitor() {

  boolean newCueDeckAState = digitalRead(CUEDeckAPin);
  boolean newCueDeckBState = digitalRead(CUEDeckBPin);

  if (newCueDeckAState != cueDeckAState) {
    if (newCueDeckAState == LOW) {
      // Tombol Cue Monitor Deck A ditekan, kirim pesan MIDI sesuai kebutuhan Anda
      MIDI.sendNoteOn(70, 127, 1);  // Contoh: NoteOn Ch 1, Note 70, Velocity 127
    } else {
      // Tombol Cue Monitor Deck A dilepas, kirim pesan MIDI sesuai kebutuhan Anda
      MIDI.sendNoteOff(70, 0, 1);  // Contoh: NoteOff Ch 1, Note 70, Velocity 0
    }
    cueDeckAState = newCueDeckAState;
  }

  if (newCueDeckBState != cueDeckBState) {
    if (newCueDeckBState == LOW) {
      // Tombol Cue Monitor Deck B ditekan, kirim pesan MIDI sesuai kebutuhan Anda
      MIDI.sendNoteOn(71, 127, 1);  // Contoh: NoteOn Ch 1, Note 71, Velocity 127
    } else {
      // Tombol Cue Monitor Deck B dilepas, kirim pesan MIDI sesuai kebutuhan Anda
      MIDI.sendNoteOff(71, 0, 1);  // Contoh: NoteOff Ch 1, Note 71, Velocity 0
    }
    cueDeckBState = newCueDeckBState;
  }
}

int smoothAnalogValue(int pin) {
  int rawValue = 0;
  for (int i = 0; i < numSamples; i++) {
    rawValue += analogRead(pin);
  }
  return rawValue / numSamples;
}

void sendMIDINoteOnOff(int channel, int note, boolean state) {
  if (state) {
    MIDI.sendNoteOn(note, 127, channel);
  } else {
    MIDI.sendNoteOff(note, 0, channel);
  }
}
