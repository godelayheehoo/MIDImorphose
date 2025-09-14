
#pragma once
#include <Adafruit_ST7789.h>

enum MenuState {
    MAIN_MENU,
    MENU_1,
    MENU_2,
    NOTE_JITTER_PROB_MENU,
    RETRIGGER_PROB_MENU,
    CHANNEL_CONFIG_MENU,
    STUTTER_LENGTH_MENU
};
// Stutter length menu options (pulse resolutions and labels)
const int NUM_STUTTER_LENGTH_OPTIONS = 16;
const int STUTTER_LENGTH_TOTAL_ITEMS = NUM_STUTTER_LENGTH_OPTIONS + 1; // 1 for '...', rest for options
static const unsigned int STUTTER_LENGTH_PULSE_RESOLUTIONS[NUM_STUTTER_LENGTH_OPTIONS] = {
    3, 4, 6, 8, 9, 12, 15, 16, 18, 24, 32, 36, 48, 64, 72, 96
};
static const char* STUTTER_LENGTH_LABELS[NUM_STUTTER_LENGTH_OPTIONS] = {
    "1/32 note", "1/32 triplet", "1/16 note", "1/16 triplet", "dotted 1/16 note", "1/8 note", "5/32 note (odd)", "1/8 triplet", "dotted 1/8 note", "1/4 note", "1/4 triplet", "dotted 1/4 note", "1/2 note", "1/2 triplet", "dotted 1/2 note", "whole note"
};


enum MenuButton {
    BUTTON_NONE,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_SELECT
};


const int NUM_MENU1_OPTIONS = 4;
const int MENU1_TOTAL_ITEMS = NUM_MENU1_OPTIONS + 1; // 1 for title, 4 for options

const int NUM_MENUB_OPTIONS = 6;
const int MENUB_TOTAL_ITEMS = NUM_MENUB_OPTIONS + 1; // 1 for title, 6 for options

const int MAIN_MENU_ITEMS = 2; // Menu 1 and Menu B
class MenuManager {
public:
    MenuManager(Adafruit_ST7789& display);
    void render();
    void handleInput(MenuButton btn);
    void handleJitterKeypad(char key); // For NOTE_JITTER_PROB_MENU
    void handleRetriggerKeypad(char key); // For RETRIGGER_PROB_MENU
    MenuState currentMenu;
public:
    Adafruit_ST7789& tft;
    // Main menu selection
    int mainMenuSelectedIdx = 0;
    int channelConfigSelectedIdx = 0;
    int mainMenuScrollIdx = 0;
    static const int MAIN_MENU_VISIBLE_ITEMS = 3;
    // Menu 1 selection
    int menu1SelectedIdx = 0; // 0 = '...', 1-4 = options
    int menu1ScrollIdx = 0; // index of first visible option (0 = '...', 1 = Option 1, ...)
    static const int MENU1_VISIBLE_OPTIONS = 4; // number of options visible at once
    int menu1ActiveIdx = 1; // Active option, initially Option 1 (index 1)
    // Menu B selection
    int menuBSelectedIdx = 0; // 0 = '...', 1-6 = options
    int menuBScrollIdx = 0;
    static const int MENUB_VISIBLE_OPTIONS = 4;
    int menuBActiveIdx = 1; // Active option, initially Option A (index 1)
    // Note Jitter Prob menu
    String jitterInputBuffer = "";
    int noteJitterProb = 0;
    String retriggerInputBuffer = "";
    int retriggerProb = 10;
    // No selection index needed; only '...' is selectable
    // Stutter Length menu selection
    int stutterLengthSelectedIdx = 0; // 0 = '...', 1-16 = options
    int stutterLengthScrollIdx = 0;
    static const int STUTTER_LENGTH_VISIBLE_OPTIONS = 3;
    int stutterLengthActiveIdx = 1; // Active option, initially Option 1 (index 1)
    // Stutter length menu-held pulse resolution
    unsigned int pulseResolution = STUTTER_LENGTH_PULSE_RESOLUTIONS[9]; // Default to 1/4 note (index 9)
};