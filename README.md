# MIDI Mangler V2

## Overview

MIDI Mangler V2 is a PlatformIO-based Arduino project for the ATmega2560 (Mega) board. It manipulates MIDI events in real time, providing features such as stuttering/looping, channel filtering, timing effects, and note jitter. The project supports hardware controls (buttons, dipswitches), multiple display types (OLED, TM1637 7-segment), and uses the FortySevenEffects MIDI library for MIDI communication.

## Features

- **Stutter/Loop MIDI Events:** Pressing the stutter button loops MIDI events for creative effects.
- **Stutter Timestretching:** Turn the speed knob to change the rate of playback for stuttered notes.
- **Channel Filtering:** Dipswitches select active MIDI channels for drums and synths independently.
- **Timing Effects:** Dipswitches control timing resolution (from 1/32 to 1/2 notes).
- **Note Jitter:** Optionally randomizes note numbers for synth channels.
- **Pitch Bend Glitching:** Random pitch bends can be triggered on synth channels.
- **Retriggering:** Notes can be retriggered with adjustable probability and timing.
- **Buffer Management:** Large buffer for MIDI events (384 events on MEGA). LED indicates buffer full.
- **MIDI Panic:** Dedicated button sends "All Notes Off" to all channels.
- **Display Support:** OLED shows channel matrix and status; TM1637 7-segment displays stretch value.


## Hardware Setup

- **Board:** Arduino Mega (ATmega2560)
- **MIDI:** Uses Serial8 for MIDI input/output.
- **Buttons:**
  - Stutter button (pin 2)
  - Panic button (pin 3)
  - Drum MIDI assign button (pin 4, red)
  - Synth MIDI assign button (pin 5, green)
  - Log button (pin 8)
- **Dipswitches:**
  - Tempo: pins 14 (ones), 15 (twos), 16 (fours), 17 (eights)
  - Offset/Scale: pins 11 (ones), 20 (twos), 12 (fours)
  - MIDI channel assignment: pins 21–38 (see code for mapping)
- **Displays:**
  - OLED (SSD1306, I2C)
  - TM1637 7-segment (pins 7 [CLK], 6 [DIO])
- **LEDs:**
  - Buffer full indicator (pin 9)

- **Switches:**
  - Retrigger switch (pin 40)
  - Synth jitter switch (pin 39)
- **Potentiometer:**
  - Stretch potentiometer (pin A17)

## Usage

1. **Stutter/Loop:** Press the stutter button to start looping MIDI events. Release to stop.
2. **Channel Assignment:** Use dipswitches and the red/green buttons to assign drum/synth channels.
3. **Timing Resolution:** Set tempo dipswitches to select pulse resolution (see table below).
4. **Note Jitter & Retrigger:** Enable via menu or code options.
- - Jitter set by black switch
- - Retrigger set by red switch
5. **MIDI Panic:** Press the panic button to send "All Notes Off" to all active channels. 
6. **Displays:** OLED shows active channels; TM1637 shows stretch value.
7. **Timestretching:** Adjust the stretch potentiometer to change the playback speed of the stuttered loop in real time.

### Timing Resolution Table

| Dipswitch Value | Pulse Resolution | Note Division         |
|-----------------|-----------------|---------------------|
| 0               | 3               | 1/32 note           |
| 1               | 4               | 1/32 triplet        |
| 2               | 6               | 1/16 note           |
| 3               | 8               | 1/16 triplet        |
| 4               | 9               | dotted 1/16 note    |
| 5               | 12              | 1/8 note            |
| 6               | 15              | 5/32 note (odd)     |
| 7               | 16              | 1/8 triplet         |
| 8               | 18              | dotted 1/8 note     |
| 9               | 24              | 1/4 note            |
| 10              | 32              | 1/4 triplet         |
| 11              | 36              | dotted 1/4 note     |
| 12              | 48              | 1/2 note            |
| 13              | 64              | 1/2 triplet         |
| 14              | 72              | dotted 1/2 note     |
| 15              | 96              | whole note          |


## Code Structure

- `src/MIDI_Glitcher.cpp`: Main application logic, event buffering, timing, display, and user input.
- `src/Tempo_Tracker.h/cpp`: Tracks MIDI clock pulses and calculates BPM.
- `lib/`: Custom and third-party libraries (e.g., CircularBuffer, TM1637).
- `include/`: Project-wide headers.

## Notes

- Buffer holds up to 384 MIDI note events.
- If buffer is full, oldest notes are dropped and LED blinks.
- MIDI Panic button kills stuck notes.
- Channel assignments and timing can be changed live.
- Extensive TODOs and architecture notes are in the code comments.
- Blue LED will light up when enough MIDI pulses (note lengths) to stutter with, stutter does not work if this threshhold is not met. 
- TRICK: note that *while stuttering* you can manipulate which channels are on. You can thus stutter the whole thing and then let drums through by changing the relevant channel to off and updating.  There's nuance here though-- the stuttered notes will remain, but you can play on top of what's stuttered, which can create cool effects
- Retriggers do **not** go in the stutter buffer
- Notes that are jittered (note-shifted) are placed into the stutter buffer **after** adjustment.


## Troubleshooting

- If OLED fails to initialize, the program halts.
- If buffer is full, LED blinks and oldest notes are dropped.
- MIDI Panic can be used to reset all notes.

## License

If you want to use this, or part of this, go for it.  Please let me know though because that'd be so cool.  If you need a more formal license, I'm happy to adopt the one you want.
