---
applyTo: '**'
---
*** Begin Patch *** Add File: c:\Users\James\Documents\PlatformIO\Projects\MIDI_ManglerV2.github\copilot-instructions.md

Copilot Instructions for MIDI_ManglerV2
Project Overview
This is a PlatformIO-based Arduino project targeting the ATmega2560 (Mega) board. The main functionality is MIDI event manipulation, including stuttering/looping, channel filtering, and timing effects. The project integrates hardware controls (buttons, dipswitches), multiple display types (OLED, TM1637 7-segment), and communicates via MIDI using the FortySevenEffects MIDI library.

Architecture
src/: Main application logic. MIDI_Glitcher.cpp is the central file, handling MIDI event buffering, timing, display updates, and user input.
lib/: Custom and third-party libraries.
jamesUtils/: Contains utility classes like CircularBuffer.h for event buffering.
TM1637/: Contains a local copy of the TM1637 7-segment display library.
include/: For project-wide header files.
test/: For PlatformIO unit tests (not currently implemented).
Key Patterns and Conventions
MIDI Instance: The MIDI library is initialized in MIDI_Glitcher.cpp using:
To use the MIDI instance in other files, declare:
Event Buffering: Uses a custom CircularBuffer<T, Capacity> template (see CircularBuffer.h) for MIDI event storage.
Display Handling: Both OLED (Adafruit SSD1306) and TM1637 7-segment displays are supported. See MIDI_Glitcher.cpp for initialization and usage.
Hardware Abstraction: Pin numbers and hardware objects are defined as constants or globals in MIDI_Glitcher.cpp.
Button/Control Logic: Button helpers and control logic are implemented as structs/classes in the main source file.
Build, Test, and Debug
Build: Use PlatformIO commands (pio run, pio upload) or build via VS Code's PlatformIO extension.
Dependencies: Managed via platformio.ini (lib_deps section). Local libraries are in lib.
Unit Testing: Place tests in test using PlatformIO's test runner. See PlatformIO docs for details.
Board/Framework: Target is megaatmega2560 with the Arduino framework.
Integration Points
MIDI Library: Uses FortySevenEffects MIDI Library (lib_deps).
Displays: Adafruit GFX/SSD1306 for OLED, local TM1637 library for 7-segment.
Custom Libraries: Utilities and display drivers are in lib, with their own README and structure.
Examples
Using CircularBuffer:
TM1637 Display:
Special Notes
Buffer Limits: MIDI event buffer is sized for 384 events on MEGA.
Pin Assignments: Changeable in source; ensure consistency with hardware.
Code Comments: Extensive TODOs and architecture notes in MIDI_Glitcher.cpp header.
If any section is unclear or missing important project-specific details, please provide feedback or specify which workflows or patterns need more documentation. 

You will notice there is an rx_message_handling file, that is deprecated.  It is not used in the project.  Please do not suggest any changes to it.  It is left in the project for reference only.


*** End Patch