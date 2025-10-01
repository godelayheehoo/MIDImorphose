#pragma once
#include <Arduino.h>

// =============================================================================
// SYSTEM CONFIGURATION FOR MIDImorphose PROJECT
// =============================================================================
// This file contains core system parameters that define buffer sizes, timing,
// and other fundamental operational characteristics of the MIDI processing system.

// -----------------------------------------------------------------------------
// BUFFER SIZES AND LIMITS
// -----------------------------------------------------------------------------
// Core event processing
const int MAX_PULSES_PER_STUTTER = 128;    // 24 pulses -> 1 quarter note
const int MAX_EVENTS = 4 * MAX_PULSES_PER_STUTTER;  // Calculated from above

// Various buffer sizes for different processing stages
const byte JITTER_BUFFER_SIZE = 128;        // Note jitter processing buffer
const byte DRUM_JITTER_BUFFER_SIZE = 128;   // Drum-specific jitter buffer
const byte PERC_BUFFER_SIZE = 128;          // Percussion processing buffer
const byte RETRIGGER_BUFFER_SIZE = 64;      // Retrigger effect buffer
const byte MAX_DELAYED_NOTES = 128;         // Maximum delayed note events
const int MAX_NOTES = 256;                  // Maximum simultaneous notes

// -----------------------------------------------------------------------------
// TIMING CONSTANTS
// -----------------------------------------------------------------------------
// User interface timing
const int DEBOUNCE_MS = 50;                         // Button debounce time
const unsigned long TEMP_DISPLAY_TIME = 2000;      // Temporary display duration (ms)
const float MIN_STRETCH_INTERVAL_UPDATE = 200;     // Minimum stretch update interval

// MIDI processing timing
const byte RETRIGGER_SYNTH_TIME = 200;     // Retrigger timing for synths (ms)
const byte RETRIGGER_DRUM_TIME = 50;       // Retrigger timing for drums (ms) 
const byte RETRIGGER_NOTE_LENGTH = 50;     // Duration of retriggered notes (ms)

// LED behavior timing
const byte BUFFER_FULL_LED_BLINK_TIME = 20;    // LED blink duration (ms)
const byte BUFFER_FULL_LED_BLINK_COUNT = 4;    // Number of blinks (x2 for on/off)
const int BLINK_PATTERN_LENGTH = 5;            // Generic LED blink pattern length

// -----------------------------------------------------------------------------
// AUDIO PROCESSING PARAMETERS
// -----------------------------------------------------------------------------
// Hardware limitations and capabilities
const int MAX_INSTRUMENTS_PER_DRUM_MACHINE = 16;   // Max drum machine instruments
const int MAX_DRUM_MACHINES = 4;                   // Max simultaneous drum machines
const byte NUM_ACTIVE_PITCHBENDS = 4;              // Simultaneous pitch bend voices

// Processing probabilities and behavior
const byte RERETRIGGER_PROB = 50;          // Probability (0-100) for re-retriggering

// -----------------------------------------------------------------------------
// MIDI CONSTANTS
// -----------------------------------------------------------------------------
const byte MIDI_NOOP = 0x00;               // MIDI no-operation value

// -----------------------------------------------------------------------------
// DISPLAY CONFIGURATION
// -----------------------------------------------------------------------------
// OLED Display dimensions
#define SCREEN_WIDTH 128                    // OLED display width in pixels
#define SCREEN_HEIGHT 64                    // OLED display height in pixels

// -----------------------------------------------------------------------------
// KEYPAD MATRIX CONFIGURATION
// -----------------------------------------------------------------------------
// Matrix keypad dimensions (used in MatrixKeypad class)
static const uint8_t KEYPAD_ROWS = 4;      // Number of keypad rows
static const uint8_t KEYPAD_COLS = 4;      // Number of keypad columns

// -----------------------------------------------------------------------------
// DEBUG AND DEVELOPMENT FLAGS
// -----------------------------------------------------------------------------
#define DEBUG                               // Enable general debug output
#define ACTIVE_NOTES_DEBUG                  // Enable active notes debugging

// -----------------------------------------------------------------------------
// UTILITY MACROS
// -----------------------------------------------------------------------------
// Range checking macro
#define BETWEEN(x, lo, hi) ((x) >= (lo) && (x) <= (hi))