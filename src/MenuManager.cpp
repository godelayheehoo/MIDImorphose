

// Save menu settings to EEPROM
#include <EEPROM.h>
#include "MenuManager.h"

const MenuHandlers menuHandlersTable[] = {
    { &MenuManager::mainMenuUp, &MenuManager::mainMenuDown, &MenuManager::mainMenuLeft, &MenuManager::mainMenuRight, &MenuManager::mainMenuSelect }, // MAIN_MENU
    { &MenuManager::menu1Up, &MenuManager::menu1Down, &MenuManager::menu1Left, &MenuManager::menu1Right, &MenuManager::menu1Select },               // MENU_1
    { &MenuManager::menu2Up, &MenuManager::menu2Down, &MenuManager::menu2Left, &MenuManager::menu2Right, &MenuManager::menu2Select },                // MENU_2
    { &MenuManager::noteJitterProbMenuUp, &MenuManager::noteJitterProbMenuDown, &MenuManager::noteJitterProbMenuLeft, &MenuManager::noteJitterProbMenuRight, &MenuManager::noteJitterProbMenuSelect }, // NOTE_JITTER_PROB_MENU
    { &MenuManager::drumJitterProbMenuUp, &MenuManager::drumJitterProbMenuDown, &MenuManager::drumJitterProbMenuLeft, &MenuManager::drumJitterProbMenuRight, &MenuManager::drumJitterProbMenuSelect }, // DRUM_JITTER_PROB_MENU
    { &MenuManager::retriggerProbMenuUp, &MenuManager::retriggerProbMenuDown, &MenuManager::retriggerProbMenuLeft, &MenuManager::retriggerProbMenuRight, &MenuManager::retriggerProbMenuSelect }, // RETRIGGER_PROB_MENU
    { &MenuManager::randomDropProbMenuUp, &MenuManager::randomDropProbMenuDown, &MenuManager::randomDropProbMenuLeft, &MenuManager::randomDropProbMenuRight, &MenuManager::randomDropProbMenuSelect }, // RANDOM_DROP_PROB_MENU
    { &MenuManager::delayNoteProbMenuUp, &MenuManager::delayNoteProbMenuDown, &MenuManager::delayNoteProbMenuLeft, &MenuManager::delayNoteProbMenuRight, &MenuManager::delayNoteProbMenuSelect }, // DELAY_NOTE_PROB_MENU
    { &MenuManager::channelConfigMenuUp, &MenuManager::channelConfigMenuDown, &MenuManager::channelConfigMenuLeft, &MenuManager::channelConfigMenuRight, &MenuManager::channelConfigMenuSelect }, // CHANNEL_CONFIG_MENU
    { &MenuManager::stutterLengthMenuUp, &MenuManager::stutterLengthMenuDown, &MenuManager::stutterLengthMenuLeft, &MenuManager::stutterLengthMenuRight, &MenuManager::stutterLengthMenuSelect }, // STUTTER_LENGTH_MENU
    { &MenuManager::offsetMenuUp, &MenuManager::offsetMenuDown, &MenuManager::offsetMenuLeft, &MenuManager::offsetMenuRight, &MenuManager::offsetMenuSelect }, // OFFSET_MENU
    { &MenuManager::stutterTemperatureMenuUp, &MenuManager::stutterTemperatureMenuDown, &MenuManager::stutterTemperatureMenuLeft, &MenuManager::stutterTemperatureMenuRight, &MenuManager::stutterTemperatureMenuSelect }, // STUTTER_TEMPERATURE_MENU
    { &MenuManager::retriggerSynthMenuUp, & MenuManager::retriggerSynthMenuDown, & MenuManager::retriggerSynthMenuLeft, & MenuManager::retriggerSynthMenuRight, & MenuManager::retriggerSynthMenuSelect }, // RETRIGGER_SYNTH_MENU
    { &MenuManager::pitchbendProbMenuUp, & MenuManager::pitchbendProbMenuDown, & MenuManager::pitchbendProbMenuLeft, & MenuManager::pitchbendProbMenuRight, & MenuManager::pitchbendProbMenuSelect }, // PITCHBEND_PROB_MENU
    { &MenuManager::delayTimesMenuUp, & MenuManager::delayTimesMenuDown, & MenuManager::delayTimesMenuLeft, & MenuManager::delayTimesMenuRight, & MenuManager::delayTimesMenuSelect } // DELAY_TIMES_MENU
};

void MenuManager::saveStutterLength(int eepromAddr) {
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.write(eepromAddr, stutterLengthActiveIdx);
}

void MenuManager::saveOffset(int eepromAddr) {
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.write(eepromAddr, offsetActiveIdx);
}

void MenuManager::saveMenu1(int eepromAddr) {
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.write(eepromAddr, menu1ActiveIdx);
}

void MenuManager::saveMenuB(int eepromAddr) {
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.write(eepromAddr, menuBActiveIdx);
}

void MenuManager::saveNoteJitterProb(int eepromAddr) {
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.write(eepromAddr, noteJitterProb);
    Serial.print("saveNoteJitterProb called, value: ");
    Serial.println(noteJitterProb);
}

void MenuManager::saveDrumJitterProb(int eepromAddr) {
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.write(eepromAddr, drumJitterProb);
    Serial.print("saveDrumJitterProb called, value: ");
    Serial.println(drumJitterProb);
}

void MenuManager::saveRetriggerProb(int eepromAddr) {
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.write(eepromAddr, retriggerProb);
}

void MenuManager::saveStutterTemperature(int eepromAddr) {
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.write(eepromAddr, stutterTemperature);
}

void MenuManager::saveSynthRetrigger(int eepromAddr) {
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.write(eepromAddr, retriggerSynths ? true : false);
}

void MenuManager::saveRandomDropProb(int eepromAddr) {
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.write(eepromAddr, randomDropProb);
}

void MenuManager::saveDelayNoteProb(int eepromAddr) {
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.write(eepromAddr, delayNoteProb);
    Serial.print("saveDelayNoteProb called, value: ");
    Serial.println(delayNoteProb);
}


void MenuManager::savePitchbendProb(int eepromAddr) {
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.put(eepromAddr, pitchbendProb);
    Serial.print("savePitchbendProb called, value: ");
    Serial.println(pitchbendProb);
}

void MenuManager::saveMinDelayTime(int eepromAddr){
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.put(eepromAddr, minDelayTime);
    Serial.print("saveMinDelayTime called, value: ");
    Serial.println(minDelayTime);
}

void MenuManager::saveMaxDelayTime(int eepromAddr){
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.put(eepromAddr, maxDelayTime);
    Serial.print("saveMaxDelayTime called, value: ");
    Serial.println(maxDelayTime);
}

MenuManager::MenuManager(Adafruit_ST7789& display) : tft(display), currentMenu(MAIN_MENU) {
    currentOffsetSet = &OFFSET_SETS[0]; // Default to No Offset
}


void MenuManager::handleInput(MenuButton btn) {
    // Map button to handler index
    int handlerIdx = -1;
    switch (btn) {
        case BUTTON_UP: handlerIdx = 0; break;
        case BUTTON_DOWN: handlerIdx = 1; break;
        case BUTTON_LEFT: handlerIdx = 2; break;
        case BUTTON_RIGHT: handlerIdx = 3; break;
        case BUTTON_SELECT: handlerIdx = 4; break;
        default: return; // Unknown button
    }
    // Call the handler from the table
    if (currentMenu >= 0 && currentMenu < (sizeof(menuHandlersTable)/sizeof(menuHandlersTable[0]))) {
        // Assuming your struct is:
        // struct MenuHandlers { MenuActionHandler up, down, left, right, select; };
        MenuActionHandler handler = nullptr;
        switch (handlerIdx) {
            case 0: handler = menuHandlersTable[currentMenu].onUp; break;
            case 1: handler = menuHandlersTable[currentMenu].onDown; break;
            case 2: handler = menuHandlersTable[currentMenu].onLeft; break;
            case 3: handler = menuHandlersTable[currentMenu].onRight; break;
            case 4: handler = menuHandlersTable[currentMenu].onSelect; break;
            default: {handler = nullptr; Serial.println("Error: Invalid handler index"); break;} // Handles unexpected values
        }
        if (handler) {
            (this->*handler)();
        }
    }
}

//standard probability menu handling
void MenuManager::handleJitterKeypad(char key) {
    handleProbKeypad(NOTE_JITTER_PROB_MENU, key, jitterInputBuffer, noteJitterProb, EEPROM_ADDR_JITTER_PROB);
}
void MenuManager::handleDrumJitterKeypad(char key) {
    handleProbKeypad(DRUM_JITTER_PROB_MENU, key, drumJitterInputBuffer, drumJitterProb, EEPROM_ADDR_DRUM_JITTER_PROB);
}

void MenuManager::handleRetriggerKeypad(char key) {
    handleProbKeypad(RETRIGGER_PROB_MENU, key, retriggerInputBuffer, retriggerProb, EEPROM_ADDR_RETRIGGER_PROB);
}

void MenuManager::handleRandomDropProbKeypad(char key){
    handleProbKeypad(RANDOM_DROP_PROB_MENU, key, randomDropInputBuffer, randomDropProb, EEPROM_ADDR_RANDOM_DROP_PROB);
}

void MenuManager::handleDelayNoteProbKeypad(char key){
    handleProbKeypad(DELAY_NOTE_PROB_MENU,key,delayNoteInputBuffer,delayNoteProb,EEPROM_ADDR_DELAY_NOTE_PROB);
}



///special keypad handling

// Call this from loop() when in STUTTER_TEMPERATURE_MENU
void MenuManager::handleStutterTemperatureKeypad(char key) {
    if (currentMenu != STUTTER_TEMPERATURE_MENU) return;
    static bool inputLocked = false;
    if (key == '*') {
        stutterTemperatureInputBuffer = "";
        inputLocked = false;
    } else if (key == '#') {
        int val = stutterTemperatureInputBuffer.toInt();
        stutterTemperature = val;
        saveStutterTemperature(EEPROM_ADDR_STUTTER_TEMPERATURE);
        stutterTemperatureInputBuffer = String(val); // Show value
        inputLocked = true;
    } else if (key >= '0' && key <= '9') {
        if (!inputLocked && stutterTemperatureInputBuffer.length() < 3) {
            stutterTemperatureInputBuffer += key;
        }
    }
    // Ignore other keys
}

void MenuManager::handlePitchbendProbKeypad(char key) {
    if (currentMenu != PITCHBEND_PROB_MENU) return;
    static bool inputLocked = false;
    if (key == '*') {
        pitchbendProbInputBuffer= "";
        inputLocked = false;
    } else if (key == '#') {
        int val = pitchbendProbInputBuffer.toInt();
        if (val > 100000) val = 100000;
        pitchbendProb = val;
        savePitchbendProb(EEPROM_ADDR_PITCHBEND_PROB);
        pitchbendProbInputBuffer = String(val); // Show clamped value
        inputLocked = true;
    } else if (key >= '0' && key <= '9') {
        if (!inputLocked && pitchbendProbInputBuffer.length() < 6) {
            pitchbendProbInputBuffer += key;
        }
    }
    // Ignore other keys
}

void MenuManager::render() {
    if (currentMenu == MAIN_MENU) {
        // Main menu: list of menus
        // Add ptchbnd ~prob before Restore Defaults
        const char* menus[15] = {"Menu 1", "Menu 2", "Note Jitter Prob", "Drum Jitter Prob", "Retrigger Prob", "Random Drop Prob", "Delay Note Prob", "StutterTemperature", "Channel Config", "Stutter Length", "Offset/Scale", "Retrigger Synth", "ptchbnd ~prob","delay times", "Restore Defaults"};
        int yStart = 10;
        tft.setTextSize(2);
        tft.setCursor(10, yStart);
        tft.setTextColor(ST77XX_WHITE);
        tft.fillScreen(ST77XX_BLACK);
        tft.print("Main Menu");
        // Main menu labels
        int itemIdx = mainMenuScrollIdx;
        int y = yStart;
        for (int visible = 0; visible < MAIN_MENU_VISIBLE_ITEMS && itemIdx < 15; ++visible, ++itemIdx) {
            tft.setCursor(20, y + 30);
            if (mainMenuSelectedIdx == itemIdx) {
                tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
            } else {
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            }
            tft.print(menus[itemIdx]);
            y += 30;
        }
        tft.setTextColor(ST77XX_WHITE);
    } else if (currentMenu == RANDOM_DROP_PROB_MENU) {
        renderProbabilityMenu("random drop prob", randomDropInputBuffer);
    } else if (currentMenu == DELAY_NOTE_PROB_MENU) {
       renderProbabilityMenu("delay note prob", delayNoteInputBuffer);
    } else if (currentMenu == DRUM_JITTER_PROB_MENU) {
        renderProbabilityMenu("drum jitter prob", drumJitterInputBuffer);
    } else if (currentMenu == NOTE_JITTER_PROB_MENU) {
        renderProbabilityMenu("note jitter prob", jitterInputBuffer);
    } else if (currentMenu == RETRIGGER_SYNTH_MENU) {
        tft.fillScreen(ST77XX_BLACK);
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(10, 10);
        tft.print("Retrigger Synth");
        int yStart = 50;
        const char* options[3] = {"...", "Enabled", "Disabled"};
        int y = yStart;
        for (int i = 0; i < 3; ++i) {
            int squareX = 10;
            int squareY = y + 6;
            int squareSize = 12;
            // Only show square for Enabled/Disabled
            if (i == 1) {
                if (retriggerSynths) {
                    tft.fillRect(squareX, squareY, squareSize, squareSize, ST77XX_MAGENTA);
                } else {
                    tft.drawRect(squareX, squareY, squareSize, squareSize, ST77XX_WHITE);
                }
            } else if (i == 2) {
                if (!retriggerSynths) {
                    tft.fillRect(squareX, squareY, squareSize, squareSize, ST77XX_MAGENTA);
                } else {
                    tft.drawRect(squareX, squareY, squareSize, squareSize, ST77XX_WHITE);
                }
            }
            tft.setCursor(squareX + squareSize + 6, y);
            if (retriggerSynthSelectedIdx == i) {
                tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
            } else {
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            }
            tft.print(options[i]);
            y += 30;
        }
        tft.setTextColor(ST77XX_WHITE);
    }else if (currentMenu == PITCHBEND_PROB_MENU) {

        tft.fillScreen(ST77XX_BLACK);
        // Title at top
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(10, 10);
        tft.print("ptchbend ~prob");

        // '...' at top, always highlighted
        tft.setTextSize(2);
        tft.setCursor(10, 40);
        tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
        tft.print("...");

        // Number in middle, always cyan
        tft.setTextSize(2);
        tft.setCursor(40, 80);
        tft.setTextColor(ST77XX_MAGENTA, ST77XX_BLACK);
        if (pitchbendProbInputBuffer.length() > 0) {
            tft.print(pitchbendProbInputBuffer);
        } else {
            tft.print("0");
        }

        // Instructions at bottom
        tft.setTextSize(1);
        tft.setTextColor(ST77XX_YELLOW);
        tft.setCursor(10, 112);
        tft.print("press # when done, press * to restart");
        tft.setCursor(10, 120);
        tft.print("out of 100k");

    }
     else if (currentMenu == OFFSET_MENU) {
        tft.fillScreen(ST77XX_BLACK);
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(10, 10);
        tft.print("offset/scale");
        int yStart = 50;
        int itemIdx = offsetScrollIdx;
        int y = yStart;
        for (int visible = 0; visible < OFFSET_VISIBLE_OPTIONS && itemIdx < OFFSET_TOTAL_ITEMS; ++visible, ++itemIdx) {
            int squareX = 10;
            int squareY = y + 6;
            int squareSize = 12;
            if (itemIdx != 0) {
                if (itemIdx == offsetActiveIdx) {
                    tft.fillRect(squareX, squareY, squareSize, squareSize, ST77XX_MAGENTA);
                } else {
                    tft.drawRect(squareX, squareY, squareSize, squareSize, ST77XX_WHITE);
                }
            }
            tft.setCursor(squareX + squareSize + 6, y);
            if (offsetSelectedIdx == itemIdx) {
                tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
            } else {
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            }
            tft.setTextSize(2);
            if (itemIdx == 0) {
                tft.print("...");
            } else {
                tft.print(OFFSET_LABELS[itemIdx - 1]);
            }
            y += 30;
        }
        tft.setTextColor(ST77XX_WHITE);
    } else if (currentMenu == STUTTER_LENGTH_MENU) {
        tft.fillScreen(ST77XX_BLACK);
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(10, 10);
        tft.print("stutter length");
        int yStart = 50;
        int itemIdx = stutterLengthScrollIdx;
        int y = yStart;
        for (int visible = 0; visible < STUTTER_LENGTH_VISIBLE_OPTIONS && itemIdx < STUTTER_LENGTH_TOTAL_ITEMS; ++visible, ++itemIdx) {
            int squareX = 10;
            int squareY = y + 6;
            int squareSize = 12;
            if (itemIdx != 0) {
                if (itemIdx == stutterLengthActiveIdx) {
                    tft.fillRect(squareX, squareY, squareSize, squareSize, ST77XX_MAGENTA);
                } else {
                    tft.drawRect(squareX, squareY, squareSize, squareSize, ST77XX_WHITE);
                }
            }
            tft.setCursor(squareX + squareSize + 6, y);
            if (stutterLengthSelectedIdx == itemIdx) {
                tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
            } else {
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            }
            tft.setTextSize(2);
            if (itemIdx == 0) {
                tft.print("...");
            } else {
                tft.print(STUTTER_LENGTH_LABELS[itemIdx - 1]);
            }
            y += 30;
        }
        tft.setTextColor(ST77XX_WHITE);
    } else if (currentMenu == CHANNEL_CONFIG_MENU) {
        tft.fillScreen(ST77XX_BLACK);
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(10, 10);
        tft.print("Channel Config");
        const char* options[4] = {"...", "set synth channels", "set drum channels", "reset to default"};
        int y = 50;
        int visibleOptions = 3;
        int scrollIdx = channelConfigScrollIdx;
        for (int i = scrollIdx; i < scrollIdx + visibleOptions && i < 4; ++i) {
            tft.setCursor(20, y + (i - scrollIdx) * 30);
            if (channelConfigSelectedIdx == i) {
                tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
            } else {
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            }
            tft.print(options[i]);
        }
        tft.setTextColor(ST77XX_WHITE);
    } else if (currentMenu == MENU_1) {
        // Draw title and options, highlight selected, scroll if needed
        const char* options[NUM_MENU1_OPTIONS] = {"Option 1", "Option 2", "Option 3", "Option 4"};
        int yStart = 10;
        int itemIdx = menu1ScrollIdx;
        int y = yStart;
        tft.fillScreen(ST77XX_BLACK);
        for (int visible = 0; visible < MENU1_VISIBLE_OPTIONS && itemIdx < MENU1_TOTAL_ITEMS; ++visible, ++itemIdx) {
            int squareX = 10;
            int squareY = y + 6;
            int squareSize = 12;
            if (itemIdx != 0) {
                if (itemIdx == menu1ActiveIdx) {
                    tft.fillRect(squareX, squareY, squareSize, squareSize, ST77XX_MAGENTA);
                } else {
                    tft.drawRect(squareX, squareY, squareSize, squareSize, ST77XX_WHITE);
                }
            }
            tft.setCursor(squareX + squareSize + 6, y);
            if (menu1SelectedIdx == itemIdx) {
                tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
            } else {
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            }
            tft.setTextSize(2);
            if (itemIdx == 0) {
                tft.print("...");
            } else {
                tft.print(options[itemIdx - 1]);
            }
            y += 30;
        }
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(2);
    } else if (currentMenu == MENU_2) {
        // Draw title and options for Menu 
        const char* options[NUM_MENUB_OPTIONS] = {"A", "B", "C", "D", "E", "F"};
        int yStart = 10;
        int itemIdx = menuBScrollIdx;
        int y = yStart;
        tft.fillScreen(ST77XX_BLACK);
        for (int visible = 0; visible < MENUB_VISIBLE_OPTIONS && itemIdx < MENUB_TOTAL_ITEMS; ++visible, ++itemIdx) {
            int squareX = 10;
            int squareY = y + 6;
            int squareSize = 12;
            if (itemIdx != 0) {
                if (itemIdx == menuBActiveIdx) {
                    tft.fillRect(squareX, squareY, squareSize, squareSize, ST77XX_MAGENTA);
                } else {
                    tft.drawRect(squareX, squareY, squareSize, squareSize, ST77XX_WHITE);
                }
            }
            tft.setCursor(squareX + squareSize + 6, y);
            if (menuBSelectedIdx == itemIdx) {
                tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
            } else {
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            }
            tft.setTextSize(2);
            if (itemIdx == 0) {
                tft.print("...");
            } else {
                tft.print(options[itemIdx - 1]);
            }
            y += 30;
        }
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(2);
    } else if (currentMenu == STUTTER_TEMPERATURE_MENU) {
        tft.fillScreen(ST77XX_BLACK);
        // Title at top
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(10, 10);
        tft.print("temperature");

        // '...' at top, always highlighted
        tft.setTextSize(2);
        tft.setCursor(10, 40);
        tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
        tft.print("...");

        // Number in middle, always cyan
        tft.setTextSize(3);
        tft.setCursor(40, 80);
        tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
        if (stutterTemperatureInputBuffer.length() > 0) {
            tft.print(stutterTemperatureInputBuffer);
        } else {
            tft.print("0");
        }

        // Instructions at bottom
        tft.setTextSize(1);
        tft.setTextColor(ST77XX_YELLOW);
        tft.setCursor(10, 120);
        tft.print("press # when done, press * to restart");
    } else if (currentMenu == RETRIGGER_PROB_MENU) {
        tft.fillScreen(ST77XX_BLACK);
        // Title at top
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(10, 10);
        tft.print("retrigger prob");

        // '...' at top, always highlighted
        tft.setTextSize(2);
        tft.setCursor(10, 40);
        tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
        tft.print("...");

        // Number in middle, always cyan
        tft.setTextSize(3);
        tft.setCursor(40, 80);
        tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
        if (retriggerInputBuffer.length() > 0) {
            tft.print(retriggerInputBuffer);
        } else {
            tft.print("0");
        }

        // Instructions at bottom
        tft.setTextSize(1);
        tft.setTextColor(ST77XX_YELLOW);
        tft.setCursor(10, 120);
        tft.print("press # when done, press * to restart");
    } else if (currentMenu == DELAY_NOTE_PROB_MENU) {
        tft.fillScreen(ST77XX_BLACK);
        // Title at top
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(10, 10);
        tft.print("delay note prob");

        // '...' at top, always highlighted
        tft.setTextSize(2);
        tft.setCursor(10, 40);
        tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
        tft.print("...");

        // Number in middle, always cyan
        tft.setTextSize(3);
        tft.setCursor(40, 80);
        tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
        if (delayNoteInputBuffer.length() > 0) {
            tft.print(delayNoteInputBuffer);
        } else {
            tft.print("0");
        }

        // Instructions at bottom
        tft.setTextSize(1);
        tft.setTextColor(ST77XX_YELLOW);
        tft.setCursor(10, 120);
        tft.print("press # when done, press * to restart");
    }

    else if (currentMenu == DELAY_TIMES_MENU){
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(10, 10);
    tft.print("delay times");

    // '...' at top
    tft.setCursor(10, 40);
    if(delayTimeSelectedIdx==0){tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);}
    else{tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);}
    tft.print("...");

    // Slider bar
    int barX = 30, barY = 70, barW = 180, barH = 8;
    tft.fillRect(barX, barY, barW, barH, ST77XX_WHITE);

    // Map delay times (0-60000 ms) to bar pixel positions
    int minDelay = 0;
    int maxDelay = 60000;
    int minPos = barX + ((minDelayTime - minDelay) * barW) / (maxDelay - minDelay);
    int maxPos = barX + ((maxDelayTime - minDelay) * barW) / (maxDelay - minDelay);


    if(delayTimeSelectedIdx==2){
    // Draw min marker
    tft.fillCircle(minPos, barY + barH / 2, 8,  delayTimeSelectedIdx== 1 ? ST77XX_MAGENTA : ST77XX_CYAN);
    tft.setCursor(minPos - 10, barY + 18);

    // Draw max marker
    tft.fillCircle(maxPos, barY + barH / 2, 9, delayTimeSelectedIdx == 2 ? ST77XX_MAGENTA : ST77XX_CYAN);
    tft.setCursor(maxPos - 10, barY + 18);
    tft.setTextColor(ST77XX_MAGENTA);
    tft.print(maxDelayTime);
}
else{
    // Draw max marker
    tft.fillCircle(maxPos, barY + barH / 2, 9, delayTimeSelectedIdx == 2 ? ST77XX_MAGENTA : ST77XX_CYAN);
    tft.setCursor(maxPos - 10, barY + 18);

    
    // Draw min marker
    tft.fillCircle(minPos, barY + barH / 2, 8,  delayTimeSelectedIdx== 1 ? ST77XX_MAGENTA : ST77XX_CYAN);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_MAGENTA);
    tft.setCursor(minPos - 10, barY + 18);
    tft.print(minDelayTime);
}

    // Optionally, draw a line between min and max
    tft.drawLine(minPos, barY + barH / 2, maxPos, barY + barH / 2, ST77XX_YELLOW);

    tft.setTextSize(1);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setCursor(10, 120);
    tft.print("you must exit to save");
    }
}


//////// abstracted functions

// Remove the saveFunc parameter and call the member function directly
void MenuManager::handleProbKeypad(
    MenuState expectedMenu,
    char key,
    String& inputBuffer,
    byte& probVar,
    int eepromAddr
) {
    if (currentMenu != expectedMenu) return;
    static bool inputLocked = false;
    if (key == '*') {
        inputBuffer = "";
        inputLocked = false;
    } else if (key == '#') {
        int val = inputBuffer.toInt();
        if (val > 100) val = 100;
        probVar = val;
        // Call the member function directly
        if (expectedMenu == NOTE_JITTER_PROB_MENU) saveNoteJitterProb(eepromAddr);
        else if (expectedMenu == DRUM_JITTER_PROB_MENU) saveDrumJitterProb(eepromAddr);
        else if (expectedMenu == RETRIGGER_PROB_MENU) saveRetriggerProb(eepromAddr);
        else if (expectedMenu == RANDOM_DROP_PROB_MENU) saveRandomDropProb(eepromAddr);
        else if (expectedMenu == DELAY_NOTE_PROB_MENU) saveDelayNoteProb(eepromAddr);
        // ...add other cases as needed...
        else{Serial.println("Unexpected menu handling, no save function set!");}
        inputBuffer = String(val); // Show clamped value
        inputLocked = true;
    } else if (key >= '0' && key <= '9') {
        if (!inputLocked && inputBuffer.length() < 3) {
            inputBuffer += key;
        }
    }
    // Ignore other keys
}

void MenuManager::renderProbabilityMenu(const char* title, const String& inputBuffer) {
    tft.fillScreen(ST77XX_BLACK);
    // Title at top
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(10, 10);
    tft.print(title);

    // '...' at top, always highlighted
    tft.setTextSize(2);
    tft.setCursor(10, 40);
    tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
    tft.print("...");

    // Number in middle, always cyan
    tft.setTextSize(3);
    tft.setCursor(40, 80);
    tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
    if (inputBuffer.length() > 0) {
        tft.print(inputBuffer);
    } else {
        tft.print("0");
    }

    // Instructions at bottom
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setCursor(10, 120);
    tft.print("press # when done, press * to restart");
}


// Handler functions for each menu (stubs)
// MAIN_MENU
void MenuManager::mainMenuUp() {
    if (mainMenuSelectedIdx > 0) {
        mainMenuSelectedIdx--;
        if (mainMenuSelectedIdx < mainMenuScrollIdx) {
            mainMenuScrollIdx = mainMenuSelectedIdx;
        }
    }
}
void MenuManager::mainMenuDown() {
    if (mainMenuSelectedIdx < 14) { // 15 items, idx 0-14
        mainMenuSelectedIdx++;
        if (mainMenuSelectedIdx > mainMenuScrollIdx + MAIN_MENU_VISIBLE_ITEMS - 1) {
            mainMenuScrollIdx = mainMenuSelectedIdx - MAIN_MENU_VISIBLE_ITEMS + 1;
        }
    }
}
void MenuManager::mainMenuLeft() {
    // No left navigation in main menu
}
void MenuManager::mainMenuRight() {
    // No right navigation in main menu
}
void MenuManager::mainMenuSelect() {
    switch (mainMenuSelectedIdx) {
        case 0:
            currentMenu = MENU_1;
            menu1SelectedIdx = 0;
            menu1ScrollIdx = 0;
            break;
        case 1:
            currentMenu = MENU_2;
            menuBSelectedIdx = 0;
            menuBScrollIdx = 0;
            break;
        case 2:
            currentMenu = NOTE_JITTER_PROB_MENU;
            jitterInputBuffer = String(noteJitterProb);
            break;
        case 3:
            currentMenu = DRUM_JITTER_PROB_MENU;
            drumJitterInputBuffer = String(drumJitterProb);
            break;
        case 4:
            currentMenu = RETRIGGER_PROB_MENU;
            retriggerInputBuffer = String(retriggerProb);
            break;
        case 5:
            currentMenu = RANDOM_DROP_PROB_MENU;
            randomDropInputBuffer = String(randomDropProb);
            break;
        case 6:
            currentMenu = DELAY_NOTE_PROB_MENU;
            delayNoteInputBuffer = String(delayNoteProb);
            break;
        case 7:
            currentMenu = STUTTER_TEMPERATURE_MENU;
            stutterTemperatureInputBuffer = String(stutterTemperature);
            break;
        case 8:
            currentMenu = CHANNEL_CONFIG_MENU;
            channelConfigSelectedIdx = 0;
            break;
        case 9:
            currentMenu = STUTTER_LENGTH_MENU;
            stutterLengthSelectedIdx = 0;
            stutterLengthScrollIdx = 0;
            break;
        case 10:
            currentMenu = OFFSET_MENU;
            offsetSelectedIdx = 0;
            offsetScrollIdx = 0;
            break;
        case 11:
            currentMenu = RETRIGGER_SYNTH_MENU;
            retriggerSynthSelectedIdx = 0;
            break;
        case 12:
            currentMenu = PITCHBEND_PROB_MENU;
            pitchbendProbInputBuffer = String(pitchbendProb);
            break;
        case 13:
            currentMenu = DELAY_TIMES_MENU;
            delayTimeSelectedIdx = 0;
            break;
        case 14:
            readyToRestoreDefaults = true;
            break;
        default:
            break;
    }
}

// MENU_1
void MenuManager::menu1Up() {
    if (menu1SelectedIdx > 0) {
        menu1SelectedIdx--;
        if (menu1SelectedIdx < menu1ScrollIdx) {
            menu1ScrollIdx = menu1SelectedIdx;
        }
    }
}
void MenuManager::menu1Down() {
    if (menu1SelectedIdx < MENU1_TOTAL_ITEMS - 1) {
        menu1SelectedIdx++;
        if (menu1SelectedIdx > menu1ScrollIdx + MENU1_VISIBLE_OPTIONS - 1) {
            menu1ScrollIdx = menu1SelectedIdx - MENU1_VISIBLE_OPTIONS + 1;
        }
    }
}
void MenuManager::menu1Left() {
    // No left navigation in menu1
}
void MenuManager::menu1Right() {
    // No right navigation in menu1
}
void MenuManager::menu1Select() {
    if (menu1SelectedIdx == 0) {
        currentMenu = MAIN_MENU;
    } else {
        menu1ActiveIdx = menu1SelectedIdx;
        saveMenu1(EEPROM_ADDR_MENU1);
    }
}

// MENU_2
void MenuManager::menu2Up() {
    if (menuBSelectedIdx > 0) {
        menuBSelectedIdx--;
        if (menuBSelectedIdx < menuBScrollIdx) {
            menuBScrollIdx = menuBSelectedIdx;
        }
    }
}
void MenuManager::menu2Down() {
    if (menuBSelectedIdx < MENUB_TOTAL_ITEMS - 1) {
        menuBSelectedIdx++;
        if (menuBSelectedIdx > menuBScrollIdx + MENUB_VISIBLE_OPTIONS - 1) {
            menuBScrollIdx = menuBSelectedIdx - MENUB_VISIBLE_OPTIONS + 1;
        }
    }
}
void MenuManager::menu2Left() {
    // No left navigation in menu2
}
void MenuManager::menu2Right() {
    // No right navigation in menu2
}
void MenuManager::menu2Select() {
    if (menuBSelectedIdx == 0) {
        currentMenu = MAIN_MENU;
    } else {
        menuBActiveIdx = menuBSelectedIdx;
        saveMenuB(EEPROM_ADDR_MENUB);
    }
}

// NOTE_JITTER_PROB_MENU
// Up does nothing in probability menus
void MenuManager::noteJitterProbMenuUp() {
    // No action
}
// Down does nothing in probability menus
void MenuManager::noteJitterProbMenuDown() {
    // No action
}
// Left does nothing in probability menus
void MenuManager::noteJitterProbMenuLeft() {
    // No action
}
// Right does nothing in probability menus
void MenuManager::noteJitterProbMenuRight() {
    // No action
}
// Select returns to main menu in probability menus
void MenuManager::noteJitterProbMenuSelect() {
    currentMenu = MAIN_MENU;
}

// DRUM_JITTER_PROB_MENU
void MenuManager::drumJitterProbMenuUp() {
    // No action
}
void MenuManager::drumJitterProbMenuDown() {
    // No action
}
void MenuManager::drumJitterProbMenuLeft() {
    // No action
}
void MenuManager::drumJitterProbMenuRight() {
    // No action
}
void MenuManager::drumJitterProbMenuSelect() {
    currentMenu = MAIN_MENU;
}

// RETRIGGER_PROB_MENU
void MenuManager::retriggerProbMenuUp() {
    // No action
}
void MenuManager::retriggerProbMenuDown() {
    // No action
}
void MenuManager::retriggerProbMenuLeft() {
    // No action
}
void MenuManager::retriggerProbMenuRight() {
    // No action
}
void MenuManager::retriggerProbMenuSelect() {
    currentMenu = MAIN_MENU;
}

// RANDOM_DROP_PROB_MENU
void MenuManager::randomDropProbMenuUp() {
    // No action
}
void MenuManager::randomDropProbMenuDown() {
    // No action
}
void MenuManager::randomDropProbMenuLeft() {
    // No action
}
void MenuManager::randomDropProbMenuRight() {
    // No action
}
void MenuManager::randomDropProbMenuSelect() {
    currentMenu = MAIN_MENU;
}

// DELAY_NOTE_PROB_MENU
void MenuManager::delayNoteProbMenuUp() {
    // No action
}
void MenuManager::delayNoteProbMenuDown() {
    // No action
}
void MenuManager::delayNoteProbMenuLeft() {
    // No action
}
void MenuManager::delayNoteProbMenuRight() {
    // No action
}
void MenuManager::delayNoteProbMenuSelect() {
    currentMenu = MAIN_MENU;
}

// CHANNEL_CONFIG_MENU
// Up: move selection up, update scroll index
void MenuManager::channelConfigMenuUp() {
    const int numOptions = 4;
    const int visibleOptions = 3;
    if (channelConfigSelectedIdx > 0) channelConfigSelectedIdx--;
    if (channelConfigSelectedIdx < channelConfigScrollIdx) channelConfigScrollIdx = channelConfigSelectedIdx;
}
// Down: move selection down, update scroll index
void MenuManager::channelConfigMenuDown() {
    const int numOptions = 4;
    const int visibleOptions = 3;
    if (channelConfigSelectedIdx < numOptions - 1) channelConfigSelectedIdx++;
    if (channelConfigSelectedIdx > channelConfigScrollIdx + visibleOptions - 1) channelConfigScrollIdx = channelConfigSelectedIdx - visibleOptions + 1;
}
// Left: no action
void MenuManager::channelConfigMenuLeft() {
    // No action
}
// Right: no action
void MenuManager::channelConfigMenuRight() {
    // No action
}
// Select: perform action based on selection
void MenuManager::channelConfigMenuSelect() {

    if (channelConfigSelectedIdx == 1) {
        pendingSynthChannelUpdate = true;
    } else if (channelConfigSelectedIdx == 2) {
        pendingDrumChannelUpdate = true;
    } else if (channelConfigSelectedIdx == 3) {
        pendingChannelDefaultsReset = true;
    } else if (channelConfigSelectedIdx == 0) {
        // '...' selected, return to main menu
        currentMenu = MAIN_MENU;
    }
}

// STUTTER_LENGTH_MENU
// Up: move selection up, update scroll index
void MenuManager::stutterLengthMenuUp() {
    if (stutterLengthSelectedIdx > stutterLengthScrollIdx) {
        stutterLengthSelectedIdx--;
    } else if (stutterLengthScrollIdx > 0) {
        stutterLengthScrollIdx--;
        stutterLengthSelectedIdx = stutterLengthScrollIdx;
    }
}
// Down: move selection down, update scroll index
void MenuManager::stutterLengthMenuDown() {
    int lastVisibleIdx = stutterLengthScrollIdx + STUTTER_LENGTH_VISIBLE_OPTIONS - 1;
    if (stutterLengthSelectedIdx < lastVisibleIdx && stutterLengthSelectedIdx < STUTTER_LENGTH_TOTAL_ITEMS - 1) {
        stutterLengthSelectedIdx++;
    } else if (lastVisibleIdx < STUTTER_LENGTH_TOTAL_ITEMS - 1) {
        stutterLengthScrollIdx++;
        stutterLengthSelectedIdx = stutterLengthScrollIdx + STUTTER_LENGTH_VISIBLE_OPTIONS - 1;
    }
}
// Left: no action
void MenuManager::stutterLengthMenuLeft() {
    // No action
}
// Right: no action
void MenuManager::stutterLengthMenuRight() {
    // No action
}
// Select: apply changes or return to main menu
void MenuManager::stutterLengthMenuSelect() {
    if (stutterLengthSelectedIdx == 0) {
        currentMenu = MAIN_MENU;
    } else {
        stutterLengthActiveIdx = stutterLengthSelectedIdx;
        pulseResolution = STUTTER_LENGTH_PULSE_RESOLUTIONS[stutterLengthSelectedIdx - 1];
        saveStutterLength(EEPROM_ADDR_STUTTER_LENGTH);
    }
}

// OFFSET_MENU
// Up: move selection up, update scroll index
void MenuManager::offsetMenuUp() {
    if (offsetSelectedIdx > offsetScrollIdx) {
        offsetSelectedIdx--;
    } else if (offsetScrollIdx > 0) {
        offsetScrollIdx--;
        offsetSelectedIdx = offsetScrollIdx;
    }
}
// Down: move selection down, update scroll index
void MenuManager::offsetMenuDown() {
    int lastVisibleIdx = offsetScrollIdx + OFFSET_VISIBLE_OPTIONS - 1;
    if (offsetSelectedIdx < lastVisibleIdx && offsetSelectedIdx < OFFSET_TOTAL_ITEMS - 1) {
        offsetSelectedIdx++;
    } else if (lastVisibleIdx < OFFSET_TOTAL_ITEMS - 1) {
        offsetScrollIdx++;
        offsetSelectedIdx = offsetScrollIdx + OFFSET_VISIBLE_OPTIONS - 1;
    }
}
// Left: no action
void MenuManager::offsetMenuLeft() {
    // No action
}
// Right: no action
void MenuManager::offsetMenuRight() {
    // No action
}
// Select: apply changes or return to main menu
void MenuManager::offsetMenuSelect() {
    if (offsetSelectedIdx == 0) {
        currentMenu = MAIN_MENU;
    } else {
        offsetActiveIdx = offsetSelectedIdx;
        currentOffsetSet = &OFFSET_SETS[offsetSelectedIdx - 1];
        saveOffset(EEPROM_ADDR_OFFSET);
    }
}

// STUTTER_TEMPERATURE_MENU
void MenuManager::stutterTemperatureMenuUp() {}
void MenuManager::stutterTemperatureMenuDown() {}
void MenuManager::stutterTemperatureMenuLeft() {}
void MenuManager::stutterTemperatureMenuRight() {}
void MenuManager::stutterTemperatureMenuSelect() {
    currentMenu = MAIN_MENU;
}

// RETRIGGER_SYNTH_MENU
// Up: move selection up
void MenuManager::retriggerSynthMenuUp() {
    if (retriggerSynthSelectedIdx > 0) retriggerSynthSelectedIdx--;
}
// Down: move selection down
void MenuManager::retriggerSynthMenuDown() {
    if (retriggerSynthSelectedIdx < 2) retriggerSynthSelectedIdx++;
}
// Left: no action
void MenuManager::retriggerSynthMenuLeft() {
    // No action
}
// Right: no action
void MenuManager::retriggerSynthMenuRight() {
    // No action
}
// Select: apply changes or return to main menu
void MenuManager::retriggerSynthMenuSelect() {
    if (retriggerSynthSelectedIdx == 0) {
        currentMenu = MAIN_MENU;
    } else if (retriggerSynthSelectedIdx == 1) {
        retriggerSynths = true;
        saveSynthRetrigger(EEPROM_ADDR_SYNTH_RETRIGGER);
    } else if (retriggerSynthSelectedIdx == 2) {
        retriggerSynths = false;
        saveSynthRetrigger(EEPROM_ADDR_SYNTH_RETRIGGER);
    }
}

// PITCHBEND_PROB_MENU
void MenuManager::pitchbendProbMenuUp() {}
void MenuManager::pitchbendProbMenuDown() {}
void MenuManager::pitchbendProbMenuLeft() {}
void MenuManager::pitchbendProbMenuRight() {}
void MenuManager::pitchbendProbMenuSelect() {
    currentMenu = MAIN_MENU;
}

// DELAY_TIMES_MENU (uses left/right)
// Up: move selection up
void MenuManager::delayTimesMenuUp() {
    if (delayTimeSelectedIdx > 0) delayTimeSelectedIdx--;
}
// Down: move selection down
void MenuManager::delayTimesMenuDown() {
    if (delayTimeSelectedIdx < 2) delayTimeSelectedIdx++;
}
// Left: decrement value for min/max delay
void MenuManager::delayTimesMenuLeft() {
    if (delayTimeSelectedIdx == 1) {
        if (minDelayTime >= 20) minDelayTime -= 100;
        else minDelayTime = 0;
    } else if (delayTimeSelectedIdx == 2) {
        if (maxDelayTime >= 200) maxDelayTime -= 100;
        else maxDelayTime = 0;
    }
}
// Right: increment value for min/max delay
void MenuManager::delayTimesMenuRight() {
    if (delayTimeSelectedIdx == 1) {
        minDelayTime += 100;
        if (minDelayTime > maxDelayTime) minDelayTime = maxDelayTime;
    } else if (delayTimeSelectedIdx == 2) {
        maxDelayTime += 100;
        if (maxDelayTime > 60000) maxDelayTime = 60000;
    }
}
// Select: save and return to main menu if '...' is selected
void MenuManager::delayTimesMenuSelect() {
    if (delayTimeSelectedIdx == 0) {
        saveMinDelayTime(EEPROM_ADDR_MIN_DELAY_TIME);
        saveMaxDelayTime(EEPROM_ADDR_MAX_DELAY_TIME);
        currentMenu = MAIN_MENU;
    }
    // else: do nothing for other selections
}
