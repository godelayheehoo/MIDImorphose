// Save menu settings to EEPROM
#include <EEPROM.h>
#include "MenuManager.h"

void MenuManager::saveStutterLength(int eepromAddr) {
    EEPROM.write(eepromAddr, stutterLengthActiveIdx);
}

void MenuManager::saveOffset(int eepromAddr) {
    EEPROM.write(eepromAddr, offsetActiveIdx);
}

void MenuManager::saveMenu1(int eepromAddr) {
    EEPROM.write(eepromAddr, menu1ActiveIdx);
}

void MenuManager::saveMenuB(int eepromAddr) {
    EEPROM.write(eepromAddr, menuBActiveIdx);
}

void MenuManager::saveNoteJitterProb(int eepromAddr) {
    EEPROM.write(eepromAddr, noteJitterProb);
}

void MenuManager::saveRetriggerProb(int eepromAddr) {
    EEPROM.write(eepromAddr, retriggerProb);
}



MenuManager::MenuManager(Adafruit_ST7789& display) : tft(display), currentMenu(MAIN_MENU) {
    currentOffsetSet = &OFFSET_SETS[0]; // Default to No Offset
}


void MenuManager::handleInput(MenuButton btn) {
    // Channel config menu: select triggers pending update flags
    extern bool pendingDrumChannelUpdate;
    extern bool pendingSynthChannelUpdate;
    if (currentMenu == CHANNEL_CONFIG_MENU) {
        // Scroll logic: 4 options, show 3 at a time
        const int numOptions = 4;
        const int visibleOptions = 3;
        static int scrollIdx = 0;
        if (btn == BUTTON_UP) {
            if (channelConfigSelectedIdx > 0) channelConfigSelectedIdx--;
            if (channelConfigSelectedIdx < scrollIdx) scrollIdx = channelConfigSelectedIdx;
        } else if (btn == BUTTON_DOWN) {
            if (channelConfigSelectedIdx < numOptions - 1) channelConfigSelectedIdx++;
            if (channelConfigSelectedIdx > scrollIdx + visibleOptions - 1) scrollIdx = channelConfigSelectedIdx - visibleOptions + 1;
        } else if (btn == BUTTON_SELECT) {
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
        // Store scrollIdx for rendering
        channelConfigScrollIdx = scrollIdx;
        return;
    }
    // Jitter menu: only '...' is selectable, select returns to main menu
    if (currentMenu == NOTE_JITTER_PROB_MENU) {
        if (btn == BUTTON_SELECT) {
            currentMenu = MAIN_MENU;
        }
        // Up/down do nothing
        return;
    }
    // Retrigger menu: only '...' is selectable, select returns to main menu
    if (currentMenu == RETRIGGER_PROB_MENU) {
        if (btn == BUTTON_SELECT) {
            currentMenu = MAIN_MENU;
        }
        // Up/down do nothing
        return;
    }
    switch (currentMenu) {
        case MAIN_MENU: {
            // Now 7 items: Menu 1, Menu 2, Note Jitter Prob, Retrigger Prob, Channel Config, Stutter Length, Offset/Scale
            if (btn == BUTTON_UP) {
                if (mainMenuSelectedIdx > 0) {
                    mainMenuSelectedIdx--;
                    if (mainMenuSelectedIdx < mainMenuScrollIdx) {
                        mainMenuScrollIdx = mainMenuSelectedIdx;
                    }
                }
            } else if (btn == BUTTON_DOWN) {
                if (mainMenuSelectedIdx < 6) {
                    mainMenuSelectedIdx++;
                    if (mainMenuSelectedIdx > mainMenuScrollIdx + MAIN_MENU_VISIBLE_ITEMS - 1) {
                        mainMenuScrollIdx = mainMenuSelectedIdx - MAIN_MENU_VISIBLE_ITEMS + 1;
                    }
                }
            } else if (btn == BUTTON_SELECT) {
                if (mainMenuSelectedIdx == 0) {
                    currentMenu = MENU_1;
                    menu1SelectedIdx = 0;
                    menu1ScrollIdx = 0;
                } else if (mainMenuSelectedIdx == 1) {
                    currentMenu = MENU_2;
                    menuBSelectedIdx = 0;
                    menuBScrollIdx = 0;
                } else if (mainMenuSelectedIdx == 2) {
                    currentMenu = NOTE_JITTER_PROB_MENU;
                    jitterInputBuffer = "";
                } else if (mainMenuSelectedIdx == 3) {
                    currentMenu = RETRIGGER_PROB_MENU;
                    retriggerInputBuffer = "";
                } else if (mainMenuSelectedIdx == 4) {
                    currentMenu = CHANNEL_CONFIG_MENU;
                    channelConfigSelectedIdx = 0;
                } else if (mainMenuSelectedIdx == 5) {
                    currentMenu = STUTTER_LENGTH_MENU;
                    stutterLengthSelectedIdx = 0;
                    stutterLengthScrollIdx = 0;
                } else if (mainMenuSelectedIdx == 6) {
                    currentMenu = OFFSET_MENU;
                    offsetSelectedIdx = 0;
                    offsetScrollIdx = 0;
                }
            }
            break;
        case OFFSET_MENU: {
            if (btn == BUTTON_SELECT) {
                if (offsetSelectedIdx == 0) {
                    currentMenu = MAIN_MENU;
                } else {
                    offsetActiveIdx = offsetSelectedIdx;
                    currentOffsetSet = &OFFSET_SETS[offsetSelectedIdx - 1];
                    saveOffset(EEPROM_ADDR_OFFSET);
                }
            } else if (btn == BUTTON_UP) {
                if (offsetSelectedIdx > offsetScrollIdx) {
                    offsetSelectedIdx--;
                } else if (offsetScrollIdx > 0) {
                    offsetScrollIdx--;
                    offsetSelectedIdx = offsetScrollIdx;
                }
            } else if (btn == BUTTON_DOWN) {
                int lastVisibleIdx = offsetScrollIdx + OFFSET_VISIBLE_OPTIONS - 1;
                if (offsetSelectedIdx < lastVisibleIdx && offsetSelectedIdx < OFFSET_TOTAL_ITEMS - 1) {
                    offsetSelectedIdx++;
                } else if (lastVisibleIdx < OFFSET_TOTAL_ITEMS - 1) {
                    offsetScrollIdx++;
                    offsetSelectedIdx = offsetScrollIdx + OFFSET_VISIBLE_OPTIONS - 1;
                }
            }
            break;
        }
        }
        case STUTTER_LENGTH_MENU: {
            // Only allow selection within visible options
            if (btn == BUTTON_SELECT) {
                if (stutterLengthSelectedIdx == 0) {
                    currentMenu = MAIN_MENU;
                } else {
                    stutterLengthActiveIdx = stutterLengthSelectedIdx;
                    pulseResolution = STUTTER_LENGTH_PULSE_RESOLUTIONS[stutterLengthSelectedIdx - 1];
                    saveStutterLength(EEPROM_ADDR_STUTTER_LENGTH);
                }
            } else if (btn == BUTTON_UP) {
                if (stutterLengthSelectedIdx > stutterLengthScrollIdx) {
                    stutterLengthSelectedIdx--;
                } else if (stutterLengthScrollIdx > 0) {
                    stutterLengthScrollIdx--;
                    stutterLengthSelectedIdx = stutterLengthScrollIdx;
                }
            } else if (btn == BUTTON_DOWN) {
                int lastVisibleIdx = stutterLengthScrollIdx + STUTTER_LENGTH_VISIBLE_OPTIONS - 1;
                if (stutterLengthSelectedIdx < lastVisibleIdx && stutterLengthSelectedIdx < STUTTER_LENGTH_TOTAL_ITEMS - 1) {
                    stutterLengthSelectedIdx++;
                } else if (lastVisibleIdx < STUTTER_LENGTH_TOTAL_ITEMS - 1) {
                    stutterLengthScrollIdx++;
                    stutterLengthSelectedIdx = stutterLengthScrollIdx + STUTTER_LENGTH_VISIBLE_OPTIONS - 1;
                }
            }
            break;
        }
        case MENU_1:
            if (btn == BUTTON_SELECT) {
                if (menu1SelectedIdx == 0) {
                    currentMenu = MAIN_MENU;
                } else {
                    menu1ActiveIdx = menu1SelectedIdx;
                    saveMenu1(EEPROM_ADDR_MENU1);
                }
            } else if (btn == BUTTON_UP) {
                if (menu1SelectedIdx > 0) {
                    menu1SelectedIdx--;
                    if (menu1SelectedIdx < menu1ScrollIdx) {
                        menu1ScrollIdx = menu1SelectedIdx;
                    }
                }
            } else if (btn == BUTTON_DOWN) {
                if (menu1SelectedIdx < MENU1_TOTAL_ITEMS - 1) {
                    menu1SelectedIdx++;
                    if (menu1SelectedIdx > menu1ScrollIdx + MENU1_VISIBLE_OPTIONS - 1) {
                        menu1ScrollIdx = menu1SelectedIdx - MENU1_VISIBLE_OPTIONS + 1;
                    }
                }
            } else if (btn == BUTTON_RIGHT && menu1SelectedIdx == 0) {
                // Only allow right navigation when '...' is highlighted
            }
            break;
        case MENU_2:
            if (btn == BUTTON_SELECT) {
                if (menuBSelectedIdx == 0) {
                    currentMenu = MAIN_MENU;
                } else {
                    menuBActiveIdx = menuBSelectedIdx;
                    saveMenuB(EEPROM_ADDR_MENUB);
                }
            } else if (btn == BUTTON_UP) {
                if (menuBSelectedIdx > 0) {
                    menuBSelectedIdx--;
                    if (menuBSelectedIdx < menuBScrollIdx) {
                        menuBScrollIdx = menuBSelectedIdx;
                    }
                }
            } else if (btn == BUTTON_DOWN) {
                if (menuBSelectedIdx < MENUB_TOTAL_ITEMS - 1) {
                    menuBSelectedIdx++;
                    if (menuBSelectedIdx > menuBScrollIdx + MENUB_VISIBLE_OPTIONS - 1) {
                        menuBScrollIdx = menuBSelectedIdx - MENUB_VISIBLE_OPTIONS + 1;
                    }
                }
            }
            break;
        default:
            break;
    }
}

// Call this from loop() when in NOTE_JITTER_PROB_MENU
void MenuManager::handleJitterKeypad(char key) {
    if (currentMenu != NOTE_JITTER_PROB_MENU) return;
    static bool inputLocked = false;
    if (key == '*') {
        jitterInputBuffer = "";
        inputLocked = false;
    } else if (key == '#') {
        int val = jitterInputBuffer.toInt();
        if (val > 100) val = 100;
        noteJitterProb = val;
        saveNoteJitterProb(EEPROM_ADDR_JITTER_PROB);
        jitterInputBuffer = String(val); // Show clamped value
        inputLocked = true;
    } else if (key >= '0' && key <= '9') {
        if (!inputLocked && jitterInputBuffer.length() < 3) {
            jitterInputBuffer += key;
        }
    }
    // Ignore other keys
}

// Call this from loop() when in RETRIGGER_PROB_MENU
void MenuManager::handleRetriggerKeypad(char key) {
    if (currentMenu != RETRIGGER_PROB_MENU) return;
    static bool inputLocked = false;
    if (key == '*') {
        retriggerInputBuffer = "";
        inputLocked = false;
    } else if (key == '#') {
        int val = retriggerInputBuffer.toInt();
        if (val > 100) val = 100;
        retriggerProb = val;
        saveRetriggerProb(EEPROM_ADDR_RETRIGGER_PROB);
        retriggerInputBuffer = String(val); // Show clamped value
        inputLocked = true;
    } else if (key >= '0' && key <= '9') {
        if (!inputLocked && retriggerInputBuffer.length() < 3) {
            retriggerInputBuffer += key;
        }
    }
    // Ignore other keys
}

void MenuManager::render() {
    if (currentMenu == MAIN_MENU) {
        // Main menu: list of menus
        const char* menus[7] = {"Menu 1", "Menu 2", "Note Jitter Prob", "Retrigger Prob", "Channel Config", "Stutter Length", "Offset/Scale"};
        int yStart = 10;
        tft.setTextSize(2);
        tft.setCursor(10, yStart);
        tft.setTextColor(ST77XX_WHITE);
        tft.fillScreen(ST77XX_BLACK);
        tft.print("Main Menu");
        int itemIdx = mainMenuScrollIdx;
        int y = yStart;
        for (int visible = 0; visible < MAIN_MENU_VISIBLE_ITEMS && itemIdx < 7; ++visible, ++itemIdx) {
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
    } else if (currentMenu == OFFSET_MENU) {
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
    } else if (currentMenu == NOTE_JITTER_PROB_MENU) {
        tft.fillScreen(ST77XX_BLACK);
        // Title at top
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(10, 10);
        tft.print("note jitter prob");

        // '...' at top, always highlighted
        tft.setTextSize(2);
        tft.setCursor(10, 40);
        tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
        tft.print("...");

        // Number in middle, always cyan
        tft.setTextSize(3);
        tft.setCursor(40, 80);
        tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
        if (jitterInputBuffer.length() > 0) {
            tft.print(jitterInputBuffer);
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
    }
}