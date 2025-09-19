#pragma once
#include <Arduino.h>

struct MidiEvent {
  byte type;  // 0x90=noteOn, 0x80=noteOff, 0x00=pseudo-event
  byte channel;
  byte note;
  byte velocity;
  byte pulseNumber;
  unsigned long playTime;  // relative time from pulse? Or maybe we should use time from the starting pulse in the ring buffer, in which case this should be abs time. Going with latter.
  bool played;
};

// -- glitch structs --
struct JitteredNote {
  byte originalNote;
  byte newNote;
  byte channel;
};


struct PercNote {
  byte originalNote;
  byte newNote;
  byte channel;
};