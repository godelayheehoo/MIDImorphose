#pragma once
#include <Arduino.h>

// =============================================================================
// PIN DEFINITIONS FOR MIDImorphose PROJECT
// =============================================================================

// -----------------------------------------------------------------------------
// MIDI SERIAL CONNECTIONS
// -----------------------------------------------------------------------------
// These are hardware serial ports, not pin numbers
#define MIDI_RX_SERIAL  Serial8  // Hardware serial for MIDI input [34]
#define MIDI_TX_SERIAL  Serial5  // Hardware serial for MIDI output [20]

// -----------------------------------------------------------------------------
// BUTTONS AND SWITCHES (Direct Teensy Pins)
// -----------------------------------------------------------------------------
#define PIN_STUTTER_SWITCH      2   // Stutter on/off switch
#define PIN_PANIC_BUTTON        3   // All notes off panic button  
#define PIN_LOG_BUTTON          8   // Log button (INPUT mode)
#define PIN_VELOCITY_COERCION   32  // Velocity coercion pattern switch
#define PIN_JITTER_SWITCH       39  // Note jitter on/off switch
#define PIN_RETRIGGER_SWITCH    40  // Retrigger on/off switch

// -----------------------------------------------------------------------------
// DISPLAY PINS
// -----------------------------------------------------------------------------
// TM1637 7-Segment Display
#define PIN_TM1637_CLK          7   // Clock pin for TM1637
#define PIN_TM1637_DIO          6   // Data I/O pin for TM1637

// TFT Display (ST7789) - Uses SPI1 (hardware SPI)
#define PIN_TFT_CS              0   // TFT chip select
#define PIN_TFT_DC              28  // TFT data/command
#define PIN_TFT_RST             -1  // TFT reset (-1 = share Arduino reset)
// SPI1 Hardware Pins (fixed on Teensy 4.1) -used for tft:
//   MOSI (SDA) = Pin 26 
//   SCK (SCL)  = Pin 27   
//   MISO       = Pin 1 (not used for TFT)

// OLED Display (SSD1306) - Uses Wire2 (I2C)
#define PIN_OLED_RESET          -1  // OLED reset pin (-1 = share Arduino reset)

// I2C Bus Pins (Wire2 - used by OLED and MCP23017 expanders)
// Wire2 Hardware Pins (fixed on Teensy 4.1):
//   SDA = Pin 25 [blue?]
//   SCL = Pin 24 [yellow?]

// -----------------------------------------------------------------------------
// ANALOG INPUTS  
// -----------------------------------------------------------------------------
#define PIN_STRETCH_POT         A17 // Stretch/timestretching potentiometer

// -----------------------------------------------------------------------------
// LEDs
// -----------------------------------------------------------------------------
#define PIN_BUFFER_FULL_LED     9   // Buffer full warning LED

// -----------------------------------------------------------------------------
// MENU CONTROL PAD (MCP23X17 Pins)
// -----------------------------------------------------------------------------
// These are MCP23X17 expander pins, not direct Teensy pins
#define MCP_MENU_RIGHT          0   // Menu navigation right
#define MCP_MENU_LEFT           1   // Menu navigation left  
#define MCP_MENU_UP             2   // Menu navigation up
#define MCP_MENU_DOWN           3   // Menu navigation down
#define MCP_MENU_SELECT         4   // Menu select button

// -----------------------------------------------------------------------------
// KEYPAD MATRIX (MCP23X17 Pins)
// -----------------------------------------------------------------------------
// Column pins (MCP23X17 expander)
#define MCP_KEYPAD_COL1         11  
#define MCP_KEYPAD_COL2         10
#define MCP_KEYPAD_COL3         9
#define MCP_KEYPAD_COL4         8

// Row pins (MCP23X17 expander)  
#define MCP_KEYPAD_ROW1         15
#define MCP_KEYPAD_ROW2         14
#define MCP_KEYPAD_ROW3         13
#define MCP_KEYPAD_ROW4         12


// -----------------------------------------------------------------------------
// PIN ARRAYS FOR EASY ITERATION
// -----------------------------------------------------------------------------
// Keypad pin arrays (for use with matrix keypad library)
static const uint8_t KEYPAD_COL_PINS[4] = {MCP_KEYPAD_COL1, MCP_KEYPAD_COL2, MCP_KEYPAD_COL3, MCP_KEYPAD_COL4};
static const uint8_t KEYPAD_ROW_PINS[4] = {MCP_KEYPAD_ROW1, MCP_KEYPAD_ROW2, MCP_KEYPAD_ROW3, MCP_KEYPAD_ROW4};

// Menu control pin array (for easy iteration if needed)
static const uint8_t MENU_CONTROL_PINS[5] = {MCP_MENU_RIGHT, MCP_MENU_LEFT, MCP_MENU_UP, MCP_MENU_DOWN, MCP_MENU_SELECT};