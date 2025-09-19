#include <stdint.h>
#include <stddef.h>
#include "CircularBuffer.h"
#include "NoteStructs.h"  // contains struct MidiEvent

template <size_t Capacity>
void reverseMidiBuffer(CircularBuffer<MidiEvent, Capacity> &buffer) {
    MidiEvent events[Capacity];
    size_t nEvents = buffer.size();
    if (nEvents == 0) return;

    for (size_t i = 0; i < nEvents; ++i)
        events[i] = buffer[i];

    struct NoteSpan {
        uint8_t note;
        uint32_t on_time;
        uint32_t off_time;
    };
    NoteSpan spans[Capacity / 2];
    size_t nSpans = 0;

    uint32_t activeOnTimes[128];
    bool activeOnValid[128] = {false};

    for (size_t i = 0; i < nEvents; ++i) {
        MidiEvent &ev = events[i];
        if (ev.on) {
            activeOnTimes[ev.note] = ev.time;
            activeOnValid[ev.note] = true;
        } else if (activeOnValid[ev.note]) {
            NoteSpan ns;
            ns.note = ev.note;
            ns.on_time = activeOnTimes[ev.note];
            ns.off_time = ev.time;
            spans[nSpans++] = ns;
            activeOnValid[ev.note] = false;
        }
    }
    if (nSpans == 0) return;

    for (size_t i = 0; i < nSpans / 2; ++i) {
        NoteSpan tmp = spans[i];
        spans[i] = spans[nSpans - 1 - i];
        spans[nSpans - 1 - i] = tmp;
    }

    uint32_t onTimes[Capacity / 2];
    size_t nOns = 0;
    for (size_t i = 0; i < nEvents; ++i)
        if (events[i].on) onTimes[nOns++] = events[i].time;

    MidiEvent reversed[Capacity];
    size_t nReversed = 0;

    for (size_t i = 0; i < nSpans; ++i) {
        uint32_t new_on_time = onTimes[i];
        uint32_t dur = spans[i].off_time - spans[i].on_time;
        uint32_t new_off_time = new_on_time + dur;

        MidiEvent evOn  = {spans[i].note, true,  new_on_time};
        MidiEvent evOff = {spans[i].note, false, new_off_time};
        reversed[nReversed++] = evOn;
        reversed[nReversed++] = evOff;
    }

    for (size_t i = 0; i < nReversed - 1; ++i) {
        for (size_t j = i + 1; j < nReversed; ++j) {
            if (reversed[j].time < reversed[i].time) {
                MidiEvent tmp = reversed[i];
                reversed[i] = reversed[j];
                reversed[j] = tmp;
            }
        }
    }

    MidiEvent dummy;
    while (!buffer.empty()) buffer.pop(dummy);

    for (size_t i = 0; i < nReversed && i < Capacity; ++i)
        buffer.push(reversed[i]);
}
