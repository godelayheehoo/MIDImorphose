
# MIDI Mangler V2

Updates todo:
- explain the rolling stutter buffer now.
- List out the defaults that restore defaults restores.


## Overview

MIDI Mangler V2 is a PlatformIO-based Arduino project for the ATmega2560 (Mega) board. It manipulates MIDI events in real time, providing features such as stuttering/looping, channel filtering, timing effects, note jitter, and retriggering. The project uses a visual menu system on a TFT display for configuration, with hardware controls for performance features. Channel assignments are set using dipswitches and confirmed via the menu. Channel settings are saved to EEPROM for persistence. The project uses the FortySevenEffects-derived teensy MIDI library for MIDI communication.

## Features

- **Stutter/Loop MIDI Events:** Toggle stutter mode using the stutter switch to loop MIDI events for creative effects.
- **Stutter Timestretching:** Adjust the stretch potentiometer to change playback speed for stuttered notes. The current stretch value is shown on the TM1637 7-segment display.
- **Channel Assignment:** Set drum and synth channel selections using dipswitches, then confirm assignment via the TFT menu (“set drum channels” / “set synth channels”). Channel settings are saved to EEPROM.
- **Timing Effects:** Dipswitches control timing resolution (from 1/32 to 1/2 notes).
- **Note Jitter:** Enable/disable jitter using the dedicated switch. Jitter probability can be set via the TFT menu using keypad input.  Note jitter shares a button with drum machine jitter, so the note (synth) jitter can be disabled in the TFT menu.
- **Drum Jitter:** Enable/disable jitter using the dedicated switch (same as for synth).  Jitter probability can be set via the TFT menu using keypad input (dedicatd probability).
- **Retriggering:** Enable/disable retriggering using the dedicated switch. Retrigger probability can be set via the TFT menu using keypad input.
- **Pitch Bend Glitching:** Random pitch bends can be triggered on synth channels.  Currently, the probability is hardcoded to be somewhat rare.  
- **Buffer Management:** Large buffer for MIDI events (384 events on MEGA). LED indicates buffer full.
- **MIDI Panic:** Dedicated button sends "All Notes Off" to all channels. OLED displays panic status and channel matrix.
- **Display Support:**
  - **TFT (ST7789):** Visual menu system for configuration and navigation.
  - **OLED (SSD1306):** Shows channel matrix and panic status.
  - **TM1637 7-segment:** Displays current stretch value.

## Hardware Setup

- **Board:** Arduino Mega (ATmega2560)
- **MIDI:** Uses Serial 8 for MIDI input and Serial 5 for MIDI output.
- **Buttons/Switches:**
  - Stutter switch (pin 2)
  - Panic button (pin 3)
  - Retrigger switch (pin 40)
  - Synth jitter switch (pin 39)
  - Log button (pin 8)
- **Dipswitches:**
  - Tempo: pins 14 (ones), 15 (twos), 16 (fours), 17 (eights)
  - Offset/Scale: pins 11 (ones), 20 (twos), 12 (fours)
  - MIDI channel assignment: pins 21–38 (see code for mapping)
- **Displays:**
  - TFT (ST7789, SPI1): Menu navigation and configuration
  - OLED (SSD1306, I2C): Channel matrix and panic status
  - TM1637 7-segment (pins 7 [CLK], 6 [DIO]): Stretch value
- **LEDs:**
  - Buffer full indicator (pin 9)
- **Potentiometer:**
  - Stretch potentiometer (pin A17)

## Usage

1. **Stutter/Loop:** Use the stutter switch to start/stop looping MIDI events.
2. **Channel Assignment:** Set drum/synth channel dipswitches, then use the TFT menu to confirm assignment (“set drum channels” / “set synth channels”). Channel settings are saved to EEPROM.
3. **Timing Resolution:** Set tempo dipswitches to select pulse resolution (see table below).
4. **Note Jitter & Retrigger:** Enable/disable via dedicated switches. Set probability via TFT menu using keypad input.
5. **MIDI Panic:** Press the panic button to send "All Notes Off" to all active channels. OLED displays panic status.
6. **Displays:**
   - TFT: Menu navigation and configuration
   - OLED: Channel matrix and panic status
   - TM1637: Stretch value
7. **Timestretching:** Adjust the stretch potentiometer to change playback speed of the stuttered loop in real time.

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

## Menu System

The TFT display provides a visual menu system for configuration:

- **Main Menu:** Navigate between configuration options (e.g., channel config, jitter probability, retrigger probability).
- **Channel Config Menu:** Select “set drum channels” or “set synth channels” to confirm dipswitch assignments.
- **Jitter/Retrigger Menus:** Enter probability values using the keypad.
- **Navigation:** Use the control pad/buttons to move between menu items and select options.

## Code Structure

- `src/MIDI_Glitcher.cpp`: Main application logic, event buffering, timing, display, and user input.
- `src/MenuManager.h/cpp`: Menu system logic, state management, rendering, and input handling.
- `src/Tempo_Tracker.h/cpp`: Tracks MIDI clock pulses and calculates BPM.
- `lib/`: Custom and third-party libraries (e.g., CircularBuffer, TM1637).
- `include/`: Project-wide headers.

## Notes

- Buffer holds up to 384 MIDI note events.
- If buffer is full, oldest notes are dropped and LED blinks.
- MIDI Panic button kills stuck notes.
- Channel assignments and timing can be changed live via dipswitches and menu.
- Channel settings are saved/restored using EEPROM.
- Extensive TODOs and architecture notes are in the code comments.
- Blue LED lights up when enough MIDI pulses (note lengths) to stutter with; stutter does not work if this threshold is not met.
- While stuttering, you can manipulate which channels are on by changing dipswitches and updating via the menu. Stuttered notes remain, but you can play on top for creative effects.
- Retriggers do **not** go in the stutter buffer.
- Notes that are jittered (note-shifted) are placed into the stutter buffer **after** adjustment.
- It *is* possible to overwhelm the system with notes, which will cause unstable clock and dropped/damaged notes.  This doesn't seem to happen with at least my normal useage, but it's something to be aware of.  With messed up clock though, I think it's only noticeable if you have something upstream of the mangler **and** something downstream (as they'll be out of sync).  This is hard to do though, as we use separate serials for tx & rx.
- Currently nothing is done with CCs AND they're not passed through. Adding pass through will be easy, I just haven't done it.  Still thinking through how to consider manipulating CCs.
- Generally speaking, setting probability to zero for a feature turns it off entirely (and thus reduces CPU load)

## Troubleshooting

- If OLED fails to initialize, the program halts.
- If buffer is full, LED blinks and oldest notes are dropped.
- MIDI Panic can be used to reset all notes.
- There's various ways that notes can get stuck on if you turn things on or off midevent. For instances,
setting the delay probability to zero before a note off for a delayed note on arrives, the note on will get stuck until the next note event. The panic button is always available. 

## License

If you want to use this, or part of this, go for it. Please let me know though because that'd be so cool. If you need a more formal license, I'm happy to adopt the one you want.
