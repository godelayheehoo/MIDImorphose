
# MIDImorphose

Updates todo:
- explain the rolling stutter buffer now.
- List out the defaults that restore defaults restores.
- update menu list
- velocity coercion switch


## Overview

MIDImorphose is a PlatformIO-based Arduino project for the Teensy 4.1 board. It manipulates MIDI events in real time, providing features such as stuttering/looping, channel filtering, timing effects, note jitter, and retriggering. The project uses a visual menu system on a TFT display for configuration, with hardware controls for performance features. Channel assignments are set using dipswitches and confirmed via the menu. Channel settings are saved to EEPROM for persistence. The project uses the FortySevenEffects-derived teensy MIDI library for MIDI communication.

## Features

- **Stutter/Loop MIDI Events:** Toggle stutter mode using the stutter switch to loop MIDI events for creative effects.
- **Stutter Timestretching:** Adjust the stretch potentiometer to change playback speed for stuttered notes. The current stretch value is shown on the TM1637 7-segment display.
- **Channel Assignment:** Set drum and synth channel selections using dipswitches, then confirm assignment via the TFT menu (“set drum channels” / “set synth channels”). Channel settings are saved to EEPROM. 
- **Timing Effects:** Dipswitches control timing resolution (from 1/32 to 1/2 notes).
- **Note Jitter:** Enable/disable jitter using the dedicated switch. Jitter probability can be set via the TFT menu using keypad input.  Note jitter shares a button with drum machine jitter, so the note (synth) jitter can be disabled in the TFT menu.
- **Drum Jitter:** Enable/disable jitter using the dedicated switch (same as for synth).  Jitter probability can be set via the TFT menu using keypad input (dedicatd probability).
- **Retriggering:** Enable/disable retriggering using the dedicated switch. Retrigger probability can be set via the TFT menu using keypad input.
- **Pitch Bend Glitching:** Random pitch bends can be triggered on synth channels.  This is not traditional probability-- you set the numerator, the denominator is 100,000. This "rolls", or has the probability tested, approximately once on every MIDI clock pulse, so 96 times per bar.  This is a lot, so you usually want to keep it slow. The channel is chosen at random from among the active channels.
- **Buffer Management:** Large buffer for MIDI events.
- **Panic:** Dedicated button sends "All Notes Off" to all channels. OLED displays panic status and channel matrix.  This also reboots the TFT screen.
- **Display Support:**
  - **TFT (ST7789):** Visual menu system for configuration and navigation.
  - **OLED (SSD1306):** Shows channel matrix, panic status, stretch value, and activated velocity coercion pattern.
  - **TM1637 7-segment:** Displays current stretch value.  

## Hardware Setup

- **Board:** Teensy 4.1
- **MIDI:** Uses Serial 8 for MIDI input and Serial 5 for MIDI output.
- **Buttons/Switches:**
  - Stutter switch (pin 2)
  - Panic button (pin 3)
  - Retrigger switch (pin 40)
  - Synth jitter switch (pin 39)
  - Velocity coercion switch (pin 32)
  - Log button (pin 8)
- **Dipswitches:**
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

8. **Reverse Stuttering**: Pressing the 'A' key while stuttering will reverse playback (end notes get a little weird)

9. **Stutter Percolation**: Notes will randomly swap places with eachother within a stutter buffer.  The "stutter temperature" parameter controls how far away the notes that can be swapped with are. 

## Menu System


The TFT display provides a visual menu system for configuration and navigation. On any menu, the 'D' key can be pressed on the keypad to return to the main menu.  The menu structure is as follows:

- **Main Menu:**
  - Menu 1
  - Menu 2
  - Note Jitter Probability
  - Drum Jitter Probability
  - Retrigger Probability
  - Random Drop Probability
  - Delay Note Probability
  - Stutter Temperature
  - Channel Config
  - Stutter Length
  - Offset/Scale
  - Retrigger Synth
  - Pitchbend Probability (ptchbnd ~prob)
  - Delay Times (min/max)
  - Restore Defaults
  - Disable All
  - Channel Modify

- **Menu 1 / Menu 2:**
  - Leftover menus from initial development I haven't removed yet. 

- **Note Jitter / Drum Jitter / Retrigger / Random Drop / Delay Note / Pitchbend Probability / Stutter Temperature Menus:**
  - Enter probability or value using the keypad. Press `#` to confirm, `*` to clear input. Only the '...' option is selectable; pressing select returns to the main menu.

- **Channel Config Menu [deprecated]:**
  - Select “set drum channels” or “set synth channels” to confirm dipswitch assignments. Can also reset to default channel assignments. This will be removed in the next hardware edition.

- **Stutter Length Menu:**
  - Select from 16 different pulse resolutions (e.g., 1/32 note, 1/16 triplet, 1/4 note, etc.).

- **Offset/Scale Menu:**
  - Select from offset sets:

| Name | Offsets (semitones) | Description |
|------|-------------------|-------------|
| No Offset | 0 | No pitch modification |
| Any Offset | 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 | All chromatic notes |
| Major | 2, 4, 5, 7, 9, 11 | Major scale |
| Minor | 2, 3, 5, 7, 8, 10 | Natural minor scale |
| Harmonic Minor | 2, 3, 5, 7, 8, 11 | Harmonic minor scale |
| Melodic Minor | 2, 3, 5, 7, 9, 11 | Melodic minor scale |
| Pentatonic Major | 2, 4, 7, 9 | Major pentatonic scale |
| Pentatonic Minor | 3, 5, 7, 10 | Minor pentatonic scale |
| Blues | 3, 5, 6, 7, 10 | Blues scale |
| Dorian | 2, 3, 5, 7, 9, 10 | Dorian mode |
| Mixolydian | 2, 4, 5, 7, 9, 10 | Mixolydian mode |
| Brett | 7, 14, 21 | Stacked fifths |
| Fourths | 5, 10, 15 | Stacked fourths |
| Whole Tone | 2, 4, 6, 8, 10 | Whole tone scale |
| Augmented | 3, 6, 9 | Minor thirds stacked |
| Sparse Triad | 3, 7 | Minor third + perfect fifth |
| Major Triad | 4, 7 | Major triad |

- **Retrigger Synth Menu:**
  - Enable or disable retriggering for synths.

- **Delay Times Menu:**
  - Adjust minimum and maximum delay times for delay effects using a double-slider interface.

- **Restore Defaults:**
  - Resets all menu-configurable settings to their default values.

- **Disable All**
  - Sets all probabilities to 0 and disables optional functions.

- **Channel Modify**
  - Sets each channel to Synth, Drum, Synth/Drum, or None.  This will fully replace the dipswitches
  in the next hardware edition.
  
**Navigation:**
- Use the control pad/buttons to move between menu items and select options.
  - Up/Down: Move selection
  - Left/Right: Scroll or adjust values (where applicable)
  - Select: Enter submenu or confirm selection

**Keypad Input:**
- For probability/value menus, use the keypad to enter values. Press `#` to confirm, `*` to clear.

All menu selections and settings are saved to EEPROM for persistence across power cycles. See the code for detailed menu logic and available options.

- Press 'A' to approximately reverse playback when stuttering.
- Press 'D' to return to main menu when on a different menu.

## Code Structure

- `src/MIDI_Glitcher.cpp`: Main application logic, event buffering, timing, display, and user input.
- `src/MenuManager.h/cpp`: Menu system logic, state management, rendering, and input handling.
- `src/Tempo_Tracker.h/cpp`: Tracks MIDI clock pulses and calculates BPM.  Note tempo tracking isn't actually used right now. 
- `lib/`: Custom and third-party libraries (e.g., CircularBuffer, TM1637).
- `include/`: Project-wide headers.

## Notes

- Buffer holds up to 512 MIDI note events.
- If buffer is full, oldest notes are dropped and LED blinks.
- MIDI Panic button kills stuck notes.
- Channel assignments and timing can be changed live via dipswitches and menu.
- Most settings are saved/restored using EEPROM.
- Extensive TODOs and architecture notes are in the code comments.
- While stuttering, you can manipulate which channels are on by changing dipswitches and updating via the menu. Stuttered notes remain, but you can play on top for creative effects.
- Retriggers do **not** go in the stutter buffer.
- Notes that are jittered (note-shifted) are placed into the stutter buffer **after** adjustment.
- It *is* possible to overwhelm the system with notes, which will cause unstable clock and dropped/damaged notes.  This doesn't seem to happen with at least my normal useage, but it's something to be aware of.  With messed up clock though, I think it's only noticeable if you have something upstream of the mangler **and** something downstream (as they'll be out of sync).  This is hard to do though, as we use separate serials for tx & rx.
- Currently nothing is done with CCs AND they're not passed through. Adding pass through will be easy, I just haven't done it.  Still thinking through how to consider manipulating CCs.
- Generally speaking, setting probability to zero for a feature turns it off entirely (and thus reduces CPU load, which is mostly relevant if you have something downstream that cares about clock).
- Stutter operates on an always-rolling buffer, while live-played notes on tracked channels are not audible while stutter is active, it is still "listening", so you can immediately re-engage the stutter after releasting it (this is a change from previous behavior).
- Glitch effects, if enabled, are always "active"-- they do not track if the sequencer is playing.  Note that clock-based effects will not advance with no new clock pulses however. This means, for instance, that if velocity coercion is on, whatever velocity value is being used for coercion will be the velocity value all notes play at while the sequencer is paused. Jittering will still take effect, etc. 


## Troubleshooting

- If OLED fails to initialize, the program halts.
- If buffer is full, LED blinks and oldest notes are dropped.
- MIDI Panic can be used to reset all notes.
- There's various ways that notes can get stuck on if you turn things on or off midevent. For instances,
setting the delay probability to zero before a note off for a delayed note on arrives, the note on will get stuck until the next note event. The panic button is always available. 

## License

If you want to use this, or part of this, go for it. Please let me know though because that'd be so cool. If you need a more formal license, I'm happy to adopt the one you want.
