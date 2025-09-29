
#pragma once
#include <Adafruit_ST7789.h>
#include "EEPROMAddresses.h"

class MenuManager;

// Table-driven menu action handler types
typedef void (MenuManager::*MenuActionHandler)();

struct MenuHandlers {
    MenuActionHandler onUp;
    MenuActionHandler onDown;
    MenuActionHandler onLeft;
    MenuActionHandler onRight;
    MenuActionHandler onSelect;
};

// Handler table declaration
extern const MenuHandlers menuHandlersTable[];


enum MenuState {
    MAIN_MENU,
    MENU_1,
    MENU_2,
    NOTE_JITTER_PROB_MENU,
    DRUM_JITTER_PROB_MENU,
    RETRIGGER_PROB_MENU,
    RANDOM_DROP_PROB_MENU,
    DELAY_NOTE_PROB_MENU,
    CHANNEL_CONFIG_MENU,
    STUTTER_LENGTH_MENU,
    OFFSET_MENU,
    STUTTER_TEMPERATURE_MENU,
    RETRIGGER_SYNTH_MENU,
    PITCHBEND_PROB_MENU,
    DELAY_TIMES_MENU
};
// Offset sets and labels
struct OffsetSet {
    const byte* offsets;
    size_t size;
};
static byte NO_OFFSETS[1] = {0};
static byte ANY_OFFSETS[12] = {0,1,2,3,4,5,6,7,8,9,10,11};
static byte MAJOR_OFFSETS[6] = {2, 4, 5, 7, 9, 11};
static byte BRETT_OFFSETS[3] = {7, 14, 21};
static OffsetSet OFFSET_SETS[4] = {
    {NO_OFFSETS, 1},
    {ANY_OFFSETS, 12},
    {MAJOR_OFFSETS, 6},
    {BRETT_OFFSETS, 3}
};
static const char* OFFSET_LABELS[4] = {
    "No Offset", "Any Offset", "Major", "Brett"
};
const int NUM_OFFSET_OPTIONS = 4;
const int OFFSET_TOTAL_ITEMS = NUM_OFFSET_OPTIONS + 1; // 1 for '...', rest for options
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
    void handleDrumJitterKeypad(char key); // For DRUM_JITTER_PROB_MENU
    void handleRetriggerKeypad(char key); // For RETRIGGER_PROB_MENU
    void handleRandomDropProbKeypad(char key); // For RANDOM_DROP_PROB_MENU
    void handleStutterTemperatureKeypad(char key); // For STUTTER_TEMPERATURE_MENU
    void handleDelayNoteProbKeypad(char key); // For DELAY_NOTE_PROB_MENU
    void handlePitchbendProbKeypad(char key); // For 
    
    void handleProbKeypad(
    MenuState expectedMenu,
    char key,
    String& inputBuffer,
    byte& probVar,
    int eepromAddr
);

    void renderProbabilityMenu(const char* title, const String& inputBuffer);

    // Handler functions for MENU_1
    void menu1Up();
    void menu1Down();
    void menu1Left();
    void menu1Right();
    void menu1Select();

    // Handler functions for MENU_2
    void menu2Up();
    void menu2Down();
    void menu2Left();
    void menu2Right();
    void menu2Select();

    // Handler functions for NOTE_JITTER_PROB_MENU
    void noteJitterProbMenuUp();
    void noteJitterProbMenuDown();
    void noteJitterProbMenuLeft();
    void noteJitterProbMenuRight();
    void noteJitterProbMenuSelect();

    // Handler functions for DRUM_JITTER_PROB_MENU
    void drumJitterProbMenuUp();
    void drumJitterProbMenuDown();
    void drumJitterProbMenuLeft();
    void drumJitterProbMenuRight();
    void drumJitterProbMenuSelect();

    // Handler functions for RETRIGGER_PROB_MENU
    void retriggerProbMenuUp();
    void retriggerProbMenuDown();
    void retriggerProbMenuLeft();
    void retriggerProbMenuRight();
    void retriggerProbMenuSelect();

    // Handler functions for RANDOM_DROP_PROB_MENU
    void randomDropProbMenuUp();
    void randomDropProbMenuDown();
    void randomDropProbMenuLeft();
    void randomDropProbMenuRight();
    void randomDropProbMenuSelect();

    // Handler functions for DELAY_NOTE_PROB_MENU
    void delayNoteProbMenuUp();
    void delayNoteProbMenuDown();
    void delayNoteProbMenuLeft();
    void delayNoteProbMenuRight();
    void delayNoteProbMenuSelect();

    // Handler functions for CHANNEL_CONFIG_MENU
    void channelConfigMenuUp();
    void channelConfigMenuDown();
    void channelConfigMenuLeft();
    void channelConfigMenuRight();
    void channelConfigMenuSelect();

    // Handler functions for STUTTER_LENGTH_MENU
    void stutterLengthMenuUp();
    void stutterLengthMenuDown();
    void stutterLengthMenuLeft();
    void stutterLengthMenuRight();
    void stutterLengthMenuSelect();

    // Handler functions for OFFSET_MENU
    void offsetMenuUp();
    void offsetMenuDown();
    void offsetMenuLeft();
    void offsetMenuRight();
    void offsetMenuSelect();

    // Handler functions for STUTTER_TEMPERATURE_MENU
    void stutterTemperatureMenuUp();
    void stutterTemperatureMenuDown();
    void stutterTemperatureMenuLeft();
    void stutterTemperatureMenuRight();
    void stutterTemperatureMenuSelect();

    // Handler functions for RETRIGGER_SYNTH_MENU
    void retriggerSynthMenuUp();
    void retriggerSynthMenuDown();
    void retriggerSynthMenuLeft();
    void retriggerSynthMenuRight();
    void retriggerSynthMenuSelect();

    // Handler functions for PITCHBEND_PROB_MENU
    void pitchbendProbMenuUp();
    void pitchbendProbMenuDown();
    void pitchbendProbMenuLeft();
    void pitchbendProbMenuRight();
    void pitchbendProbMenuSelect();

    // Handler functions for DELAY_TIMES_MENU (uses left/right)
    void delayTimesMenuUp();
    void delayTimesMenuDown();
    void delayTimesMenuLeft();
    void delayTimesMenuRight();
    void delayTimesMenuSelect();

    MenuState currentMenu;
    // Flag for requesting channel defaults reset
    bool pendingChannelDefaultsReset = false;
    // Scroll index for channel config menu
    int channelConfigScrollIdx = 0;
    // Save functions for each menu setting
    void saveStutterLength(int eepromAddr);
    void saveOffset(int eepromAddr);
    void saveMenu1(int eepromAddr);
    void saveMenuB(int eepromAddr);
    void saveNoteJitterProb(int eepromAddr);
    void saveDrumJitterProb(int eepromAddr);
    void saveRetriggerProb(int eepromAddr);
    void saveStutterTemperature(int eepromAddr);
    void saveSynthRetrigger(int eepromAddr);
    void saveRandomDropProb(int eepromAddr);
    void saveDelayNoteProb(int eepromAddr);
    void savePitchbendProb(int eepromAddr);
    void saveMinDelayTime(int eepromAddr);
    void saveMaxDelayTime(int eepromAddr);
    // Flag to request restoreDefaults from main loop
    bool readyToRestoreDefaults = false;
    bool readyToDisableAll = false;
 
public:
    Adafruit_ST7789& tft;
    // Main menu selection
    int mainMenuSelectedIdx = 0;
    int channelConfigSelectedIdx = 0;
    int mainMenuScrollIdx = 0;
    static const int MAIN_MENU_VISIBLE_ITEMS = 3;
    // Retrigger Synth menu selection
    int retriggerSynthSelectedIdx = 0; // 0 = '...', 1 = Enabled, 2 = Disabled
    static const int RETRIGGER_SYNTH_TOTAL_ITEMS = 3;
    // Retrigger synths enabled/disabled
    bool retriggerSynths = false;
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
    byte noteJitterProb = 0;
    String drumJitterInputBuffer = "";
    byte drumJitterProb = 0;
    String retriggerInputBuffer = "";
    byte retriggerProb = 10;
    // Stutter Temperature menu
    String stutterTemperatureInputBuffer = "";
    byte stutterTemperature = 0;
    // Random Drop Probability menu
    String randomDropInputBuffer = "";
    byte randomDropProb = 0;
    // Delay Note Probability menu
    String delayNoteInputBuffer = "";
    byte delayNoteProb = 0;
    // Pitchbend Probability menu
    String pitchbendProbInputBuffer = "";
    unsigned int pitchbendProb = 0;
    // No selection index needed; only '...' is selectable
    // Stutter Length menu selection
    int stutterLengthSelectedIdx = 0; // 0 = '...', 1-16 = options
    int stutterLengthScrollIdx = 0;
    static const int STUTTER_LENGTH_VISIBLE_OPTIONS = 3;
    int stutterLengthActiveIdx = 1; // Active option, initially Option 1 (index 1)
    // Stutter length menu-held pulse resolution
    unsigned int pulseResolution = STUTTER_LENGTH_PULSE_RESOLUTIONS[9]; // Default to 1/4 note (index 9)
    // Offset menu selection
    int offsetSelectedIdx = 0; // 0 = '...', 1-4 = options
    int offsetScrollIdx = 0;
    static const int OFFSET_VISIBLE_OPTIONS = 3;
    int offsetActiveIdx = 1; // Active option, initially Option 1 (index 1)
    // Pointer to current offset set
    OffsetSet* currentOffsetSet; // Will be initialized in constructor

    unsigned int minDelayTime = 500;
    unsigned int maxDelayTime = 4000;
    byte delayTimeSelectedIdx = 0; // 0 = '...', 1 = left, 2 = right

    bool pendingSynthChannelUpdate = false;
    bool pendingDrumChannelUpdate = false;

    void mainMenuUp();
    void mainMenuDown();
    void mainMenuLeft();
    void mainMenuRight();
    void mainMenuSelect();
};