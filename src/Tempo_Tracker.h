#pragma once

#include <Arduino.h>
#include <MIDI.h>

#include <CircularBuffer.h>

struct TempoTracker {
    byte PPQN = 24; // pulses per quarter note, standard MIDI clock resolution
    CircularBuffer<unsigned long, 32> pulseTimes; // buffer for pulse times
    float bpm = 0.0; // last calculated BPM

    // Add a pulse timestamp (call this when a MIDI clock pulse is received)
    void addPulse(unsigned long timestamp) {
        pulseTimes.push(timestamp);
    }

    // Calculate average of differences between all subsequent pulses in buffer
    unsigned long averagePulseInterval() {
    if (pulseTimes.size() < 2){
        Serial.println(F("Cannot calculate bpm yet!"));
        return 0;
    }    

    unsigned long totalInterval = 0;
    for (size_t i = 1; i < pulseTimes.size(); i++) {
        totalInterval += pulseTimes[i] - pulseTimes[i-1];
    }
    return totalInterval / (pulseTimes.size() - 1);
}

    // Calculate bpm from average interval, assuming PPQN pulses per quarter note, to nearest integer
    float calculateBPM() {
        unsigned long avgInterval = averagePulseInterval();
        if (avgInterval == 0) {
            return 0; // cannot calculate BPM
        }
        bpm = 60000.0 / (avgInterval * PPQN);
        return bpm;
    }
};