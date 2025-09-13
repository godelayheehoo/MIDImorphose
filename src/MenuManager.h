
#pragma once
#include <Adafruit_ST7789.h>

enum MenuState {
    MAIN_MENU,
    MENU_1,
    MENU_2,
    NOTE_JITTER_PROB_MENU
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
    MenuState currentMenu;
private:
    Adafruit_ST7789& tft;
    // Main menu selection
    int mainMenuSelectedIdx = 0;
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
};