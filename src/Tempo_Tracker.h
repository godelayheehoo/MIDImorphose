#pragma once

#include <Arduino.h>
#include <MIDI.h>
#include <CircularBuffer.h>

//import the MIDI object from MIDI_Glitcher.cpp
extern MIDI_NAMESPACE::MidiInterface<HardwareSerial> MIDI;

struct TempoTracker {
    byte PPQN = 24; // pulses per quarter note, standard MIDI clock resolution
    CircularBuffer<unsigned long, 32> pulseTimes; // buffer for pulse times
    float bpm = 0.0; // last calculated BPM

    // Add a pulse timestamp (call this when a MIDI clock pulse is received)
    void addPulse(unsigned long timestamp);

    // Calculate average of differences between all subsequent pulses in buffer
    unsigned long averagePulseInterval();

    // Calculate bpm from average interval, assuming PPQN pulses per quarter note
    float calculateBPM();
};
   