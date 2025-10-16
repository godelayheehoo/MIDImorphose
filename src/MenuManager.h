
#pragma once
#include <Adafruit_ST7789.h>
#include "EEPROMAddresses.h"
#include "SystemConfig.h"

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
    DELAY_TIMES_MENU,
    CHANNEL_MODIFY_MENU,
    PRIMARY_JITTER_MENU
};



enum ChannelModifyItems {
    S,
    D,
    S_D,
    NONE,
    NUM_CHANNEL_MODIFY_ITEMS
};

// Offset sets and labels
struct OffsetSet {
    const byte* offsets;
    size_t size;
};
static byte NO_OFFSETS[1] = {0};
static byte ANY_OFFSETS[11] = {1,2,3,4,5,6,7,8,9,10,11};
static byte MAJOR_OFFSETS[6] = {2, 4, 5, 7, 9, 11};
static byte MINOR_OFFSETS[6] = {2, 3, 5, 7, 8, 10};
static byte HARMONIC_MINOR_OFFSETS[6] = {2, 3, 5, 7, 8, 11};
static byte MELODIC_MINOR_OFFSETS[6] = {2, 3, 5, 7, 9, 11};
static byte PENTATONIC_MAJOR_OFFSETS[4] = {2, 4, 7, 9};
static byte PENTATONIC_MINOR_OFFSETS[4] = {3, 5, 7, 10};
static byte BLUES_OFFSETS[5] = {3, 5, 6, 7, 10};
static byte DORIAN_OFFSETS[6] = {2, 3, 5, 7, 9, 10};
static byte MIXOLYDIAN_OFFSETS[6] = {2, 4, 5, 7, 9, 10};
static byte BRETT_OFFSETS[3] = {7, 14, 21}; //stacking fifths
static byte FOURTHS_OFFSETS[3] = {5, 10, 15}; // stacked fourths
static byte WHOLE_TONE_OFFSETS[5] = {2, 4, 6, 8, 10}; // whole tone scale
static byte AUGMENTED_OFFSETS[3] = {3, 6, 9};         // minor 3rds stacked
static byte SPARSE_TRIAD_OFFSETS[2] = {3, 7};  // minor third + perfect fifth
static byte MAJOR_TRIAD_OFFSETS[2] = {4, 7};   // major triad
static OffsetSet OFFSET_SETS[17] = {
    {NO_OFFSETS, 1},
    {ANY_OFFSETS, 11},
    {MAJOR_OFFSETS, 6},
    {MINOR_OFFSETS, 6},
    {HARMONIC_MINOR_OFFSETS, 6},
    {MELODIC_MINOR_OFFSETS, 6},
    {PENTATONIC_MAJOR_OFFSETS, 4},
    {PENTATONIC_MINOR_OFFSETS, 4},
    {BLUES_OFFSETS, 5},
    {DORIAN_OFFSETS, 6},
    {MIXOLYDIAN_OFFSETS, 6},
    {BRETT_OFFSETS, 3},
    {FOURTHS_OFFSETS, 3},
    {WHOLE_TONE_OFFSETS, 5},
    {AUGMENTED_OFFSETS, 3},
    {SPARSE_TRIAD_OFFSETS, 2},
    {MAJOR_TRIAD_OFFSETS, 2}
};
static const char* OFFSET_LABELS[17] = {
    "No Offset", "Any Offset", "Major", "Minor", "Harmonic Minor", "Melodic Minor", "Pentatonic Major", "Pentatonic Minor", "Blues", "Dorian", "Mixolydian", "Brett", "Fourths", "Whole Tone", "Augmented", "Sparse Triad", "Major Triad"
};
const int NUM_OFFSET_OPTIONS = 17;
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

const int PRIMARY_JITTER_TOTAL_ITEMS = 2;
static const char* PRIMARY_JITTER_LABELS[PRIMARY_JITTER_TOTAL_ITEMS] = {"Note Jitter", "Drum Jitter"};

enum MenuButton {
    BUTTON_NONE,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_SELECT
};



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

    // Handler functions for CHANNEL_MODIFY_MENU
    void setChannelModifyOption();
    void channelModifyMenuUp();
    void channelModifyMenuDown();
    void channelModifyMenuLeft();
    void channelModifyMenuRight();
    void channelModifyMenuSelect();

    // Handler functions for PRIMARY_JITTER_MENU
    void primaryJitterMenuUp();
    void primaryJitterMenuDown();
    void primaryJitterMenuLeft();
    void primaryJitterMenuRight();
    void primaryJitterMenuSelect();

    MenuState currentMenu;
    // Flag for requesting channel defaults reset
    bool pendingChannelDefaultsReset = false;
    // Scroll index for channel config menu
    int channelConfigScrollIdx = 0;
    // Save functions for each menu setting
    void saveStutterLength(int eepromAddr);
    void saveOffset(int eepromAddr);

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
    void saveDrumState(int eepromAddr);
    void saveSynthState(int eepromAddr);
    void saveChannelStates();
    // Flag to request restoreDefaults from main loop
    bool readyToRestoreDefaults = false;
    bool readyToDisableAll = false;
 
    Adafruit_ST7789& tft;
    // Main menu selection
    int mainMenuSelectedIdx = 0;
    int channelConfigSelectedIdx = 0;
    int mainMenuScrollIdx = 0;
    static const int MAIN_MENU_VISIBLE_ITEMS = SAFE_VISIBLE_OPTIONS;
    // Retrigger Synth menu selection
    int retriggerSynthSelectedIdx = 0; // 0 = '...', 1 = Enabled, 2 = Disabled
    static const int RETRIGGER_SYNTH_TOTAL_ITEMS = 3;
    // Retrigger synths enabled/disabled
    bool retriggerSynths = false;


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
    static const int STUTTER_LENGTH_VISIBLE_OPTIONS = SAFE_VISIBLE_OPTIONS;
    int stutterLengthActiveIdx = 1; // Active option, initially Option 1 (index 1)
    // Stutter length menu-held pulse resolution
    unsigned int pulseResolution = STUTTER_LENGTH_PULSE_RESOLUTIONS[9]; // Default to 1/4 note (index 9)
    // Offset menu selection
    int offsetSelectedIdx = 0; // 0 = '...', 1-4 = options
    int offsetScrollIdx = 0;
    static const int OFFSET_VISIBLE_OPTIONS = SAFE_VISIBLE_OPTIONS;
    int offsetActiveIdx = 1; // Active option, initially Option 1 (index 1)
    // Pointer to current offset set
    OffsetSet* currentOffsetSet; // Will be initialized in constructor

    unsigned int minDelayTime = 500;
    unsigned int maxDelayTime = 4000;
    byte delayTimeSelectedIdx = 0; // 0 = '...', 1 = left, 2 = right

    bool pendingSynthChannelUpdate = false;
    bool pendingDrumChannelUpdate = false;

    byte channelModifyHorizontalIdx = 0; // 0>1,  channels 1-16
    byte channelModifyVerticalIdx = 0;
    byte currentModifyOption = NONE; // 0=S, 1=2, 2=S_D, 3=NONE

    static const int CHANNEL_MODIFY_TOTAL_ITEMS = 16; // 1 for '...',

    bool drumMIDIenabled[16]  = {
        false, false, false, false,
        false, false, false, false,
        false, true, true, false,
        false, false, false, false
        };

    bool synthMIDIenabled[16]=  {
    true, true, true, true,
    true, true, true, true,
    true, false, false, true,
    true, true, false, false
    };

    uint16_t oldSynthState = 0;
    uint16_t newSynthState = 0;
    uint16_t oldDrumState = 0;
    uint16_t newDrumState = 0;

    bool redrawSDMatrix = false;

    byte primaryJitterSelectedIdx=0;


    void mainMenuUp();
    void mainMenuDown();
    void mainMenuLeft();
    void mainMenuRight();
    void mainMenuSelect();
};