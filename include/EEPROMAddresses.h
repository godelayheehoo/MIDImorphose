#pragma once
//EEPROM magic number to indicate valid stored settings
#define EEPROM_MAGIC 0x11

// EEPROM address constants for all menu settings and states
#define EEPROM_ADDR_MAGIC          0
#define EEPROM_ADDR_DRUM_STATE     1
#define EEPROM_ADDR_SYNTH_STATE    17
#define EEPROM_ADDR_STUTTER_LENGTH 33
#define EEPROM_ADDR_OFFSET         34
#define EEPROM_ADDR_JITTER_PROB    35
#define EEPROM_ADDR_RETRIGGER_PROB 36
#define EEPROM_ADDR_STUTTER_TEMPERATURE 37
#define EEPROM_ADDR_SYNTH_RETRIGGER 38
#define EEPROM_ADDR_DRUM_JITTER_PROB 39
#define EEPROM_ADDR_RANDOM_DROP_PROB 40
#define EEPROM_ADDR_DELAY_NOTE_PROB 41
#define EEPROM_ADDR_PITCHBEND_PROB 42
//NOTE: PITCHBEND PROB IS AN (unsigned) INT! NOT A BYTE! Ints on teensy take 4 bytes, on megas they take 2.
// so the next address should be 48.  We'll just lose the space if it's on a mega I guess.  Can change that later 
//presumably.  You muse use EEPROM.get and EEPROM.put to read and write ints.
#define EEPROM_ADDR_MIN_DELAY_TIME 46 
#define EEPROM_ADDR_MAX_DELAY_TIME 50
// New address for delay times (4 bytes each for min and max), so next address is 56
