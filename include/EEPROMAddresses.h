#pragma once
//EEPROM magic number to indicate valid stored settings
#define EEPROM_MAGIC 0x09

// EEPROM address constants for all menu settings and states
#define EEPROM_ADDR_MAGIC          0
#define EEPROM_ADDR_DRUM_STATE     1
#define EEPROM_ADDR_SYNTH_STATE    17
#define EEPROM_ADDR_STUTTER_LENGTH 33
#define EEPROM_ADDR_OFFSET         34
#define EEPROM_ADDR_MENU1          35
#define EEPROM_ADDR_MENUB          36
#define EEPROM_ADDR_JITTER_PROB    37
#define EEPROM_ADDR_RETRIGGER_PROB 38
#define EEPROM_ADDR_STUTTER_TEMPERATURE 39
#define EEPROM_ADDR_SYNTH_RETRIGGER 40
#define EEPROM_ADDR_DRUM_JITTER_PROB 41
#define EEPROM_ADDR_RANDOM_DROP_PROB 42
#define EEPROM_ADDR_DELAY_NOTE_PROB 43
#define EEPROM_ADDR_PITCHBEND_PROB 44
//NOTE: PITCHBEND PROB IS AN (unsigned) INT! NOT A BYTE! Ints on teensy take 4 bytes, on megas they take 2.
// so the next address should be 48.  We'll just lose the space if it's on a mega I guess.  Can change that later 
//presumably. 

