#pragma once
//EEPROM magic number to indicate valid stored settings
#define EEPROM_MAGIC 0x03

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
