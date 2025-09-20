#pragma once
#include <stdint.h>
#include <stddef.h>
#include "CircularBuffer.h"
#include "NoteStructs.h"  // contains struct MidiEvent
#include <MIDI.h>         // optional, we use raw status bytes below

// MIDI status bytes (we store "type" as 0x90 / 0x80 in your MidiEvent)
constexpr uint8_t MIDI_NOTE_ON  = 0x90;
constexpr uint8_t MIDI_NOTE_OFF = 0x80;

template <size_t Capacity>
void reverseMidiBuffer(CircularBuffer<MidiEvent, Capacity> &buffer) {
    if (buffer.size() == 0) return;

    // --- copy out to linear array ---
    MidiEvent events[Capacity];
    size_t nEvents = buffer.size();
    for (size_t i = 0; i < nEvents; ++i) events[i] = buffer[i];

    // --- helper sizes ---
    constexpr size_t MIDI_CHANNELS = 16;
    constexpr size_t MIDI_NOTES = 128;

    // --- NoteSpan stores all data we need to reconstruct on/off events ---
    struct NoteSpan {
        uint8_t note;
        uint8_t channel;         // preserved as-is (1..16 or 0..15 depending on your code)
        uint32_t on_time;
        uint32_t off_time;
        uint8_t on_velocity;
        uint8_t off_velocity;
        uint8_t on_pulse;
        uint8_t off_pulse;
    };

    NoteSpan spans[Capacity / 2];
    size_t nSpans = 0;

    // Active-on state keyed by [channel][note]
    // We'll index channels as 0..15 (try to map 1..16 -> 0..15)
    uint32_t activeOnTimes[MIDI_CHANNELS][MIDI_NOTES];
    bool     activeOnValid[MIDI_CHANNELS][MIDI_NOTES];
    uint8_t  activeOnVelocity[MIDI_CHANNELS][MIDI_NOTES];
    uint8_t  activeOnPulse[MIDI_CHANNELS][MIDI_NOTES];

    // initialize
    for (size_t ch = 0; ch < MIDI_CHANNELS; ++ch)
        for (size_t n = 0; n < MIDI_NOTES; ++n)
            activeOnValid[ch][n] = false;

    auto channelIndex = [](uint8_t ch)->size_t {
        // accommodate both 1..16 and 0..15 conventions:
        if (ch >= 1 && ch <= 16) return static_cast<size_t>(ch - 1);
        return static_cast<size_t>(ch % 16);
    };

    // --- build note spans from events (match ON -> OFF by channel+note) ---
    for (size_t i = 0; i < nEvents; ++i) {
        const MidiEvent &ev = events[i];

        // determine whether this is ON or OFF (treat NoteOn with vel==0 as NoteOff)
        bool isOn  = (ev.type == MIDI_NOTE_ON && ev.velocity != 0);
        bool isOff = (ev.type == MIDI_NOTE_OFF) || (ev.type == MIDI_NOTE_ON && ev.velocity == 0);

        size_t ch = channelIndex(ev.channel);
        uint8_t note = ev.note;

        if (isOn) {
            activeOnTimes[ch][note]    = ev.playTime;
            activeOnVelocity[ch][note] = ev.velocity;
            activeOnPulse[ch][note]    = ev.pulseNumber;
            activeOnValid[ch][note]    = true;
        } else if (isOff) {
            if (activeOnValid[ch][note]) {
                NoteSpan ns;
                ns.note         = note;
                ns.channel      = ev.channel; // keep original channel numbering
                ns.on_time      = activeOnTimes[ch][note];
                ns.off_time     = ev.playTime;
                ns.on_velocity  = activeOnVelocity[ch][note];
                ns.off_velocity = ev.velocity;
                ns.on_pulse     = activeOnPulse[ch][note];
                ns.off_pulse    = ev.pulseNumber;
                spans[nSpans++] = ns;

                activeOnValid[ch][note] = false;
            } else {
                // Off with no matching On -> ignore (you can synthesize an On if you prefer)
            }
        } else {
            // other event types (pseudo-events) are ignored in span building
        }
    }

    // --- synthesize OFFs for lingering ONs (optional improvement we added) ---
    if (nEvents > 0) {
        uint32_t lastTime = events[nEvents - 1].playTime;
        for (size_t ch = 0; ch < MIDI_CHANNELS; ++ch) {
            for (size_t note = 0; note < MIDI_NOTES; ++note) {
                if (activeOnValid[ch][note]) {
                    NoteSpan ns;
                    ns.note         = static_cast<uint8_t>(note);
                    // reconstruct channel numbering the same way we stored it
                    // find an example event with that channel to keep numbering consistent:
                    // (simpler: use ch+1 if you used 1..16, else ch)
                    ns.channel      = static_cast<uint8_t>((ch < 16) ? (ch + 1) : ch);
                    ns.on_time      = activeOnTimes[ch][note];
                    ns.off_time     = lastTime + 1;
                    ns.on_velocity  = activeOnVelocity[ch][note];
                    ns.off_velocity = 0;         // synthetic off
                    ns.on_pulse     = activeOnPulse[ch][note];
                    ns.off_pulse    = 0;
                    spans[nSpans++] = ns;
                    activeOnValid[ch][note] = false;
                }
            }
        }
    }

    if (nSpans == 0) return;

    // --- reverse spans order (playback order reversed) ---
    for (size_t i = 0; i < nSpans / 2; ++i) {
        NoteSpan tmp = spans[i];
        spans[i] = spans[nSpans - 1 - i];
        spans[nSpans - 1 - i] = tmp;
    }

    // --- collect original on-times (chronological) ---
    uint32_t onTimes[Capacity / 2];
    size_t nOns = 0;
    for (size_t i = 0; i < nEvents; ++i) {
        const MidiEvent &ev = events[i];
        bool isOn = (ev.type == MIDI_NOTE_ON && ev.velocity != 0);
        if (isOn) onTimes[nOns++] = ev.playTime;
    }

    // defensive: if we somehow have fewer onTimes than spans, clamp spans count
    if (nOns < nSpans) nSpans = nOns;

    // --- build reversed MidiEvent list, preserving fields ---
    MidiEvent reversed[Capacity];
    size_t nReversed = 0;

    for (size_t i = 0; i < nSpans; ++i) {
        uint32_t new_on_time = onTimes[i];
        uint32_t dur = spans[i].off_time - spans[i].on_time;
        uint32_t new_off_time = new_on_time + dur;

        // build NoteOn
        MidiEvent evOn;
        evOn.type        = MIDI_NOTE_ON;
        evOn.channel     = spans[i].channel;
        evOn.note        = spans[i].note;
        evOn.velocity    = spans[i].on_velocity;
        evOn.pulseNumber = spans[i].on_pulse;
        evOn.playTime    = new_on_time;
        evOn.played      = false;
        reversed[nReversed++] = evOn;

        // build NoteOff
        MidiEvent evOff;
        evOff.type        = MIDI_NOTE_OFF;
        evOff.channel     = spans[i].channel;
        evOff.note        = spans[i].note;
        evOff.velocity    = spans[i].off_velocity;
        evOff.pulseNumber = spans[i].off_pulse;
        evOff.playTime    = new_off_time;
        evOff.played      = false;
        reversed[nReversed++] = evOff;
    }

    // --- sort by playTime (simple O(n^2) sort for small N) ---
    for (size_t i = 0; i + 1 < nReversed; ++i) {
        for (size_t j = i + 1; j < nReversed; ++j) {
            if (reversed[j].playTime < reversed[i].playTime) {
                MidiEvent tmp = reversed[i];
                reversed[i] = reversed[j];
                reversed[j] = tmp;
            }
        }
    }

    // --- clear and repopulate ring buffer ---
    MidiEvent tmpEv;
    while (!buffer.empty()) buffer.pop(tmpEv);
    for (size_t i = 0; i < nReversed && i < Capacity; ++i) buffer.push(reversed[i]);
}
    