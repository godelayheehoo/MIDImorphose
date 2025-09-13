#include "MenuManager.h"

MenuManager::MenuManager(Adafruit_ST7789& display) : tft(display), currentMenu(MAIN_MENU) {}


void MenuManager::handleInput(MenuButton btn) {
    switch (currentMenu) {
        case MAIN_MENU:
            if (btn == BUTTON_UP) {
                if (mainMenuSelectedIdx > 0) mainMenuSelectedIdx--;
            } else if (btn == BUTTON_DOWN) {
                if (mainMenuSelectedIdx < MAIN_MENU_ITEMS - 1) mainMenuSelectedIdx++;
            } else if (btn == BUTTON_SELECT) {
                if (mainMenuSelectedIdx == 0) {
                    currentMenu = MENU_1;
                    menu1SelectedIdx = 0;
                    menu1ScrollIdx = 0;
                } else if (mainMenuSelectedIdx == 1) {
                    currentMenu = MENU_2;
                    menuBSelectedIdx = 0;
                    menuBScrollIdx = 0;
                }
            }
            break;
        case MENU_1:
            if (btn == BUTTON_SELECT) {
                if (menu1SelectedIdx == 0) {
                    currentMenu = MAIN_MENU;
                } else {
                    menu1ActiveIdx = menu1SelectedIdx;
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


void MenuManager::render() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextSize(2);
    if (currentMenu == MAIN_MENU) {
        // Main menu: list of menus
        const char* menus[MAIN_MENU_ITEMS] = {"Menu 1", "Menu 2"};
        int yStart = 10;
        tft.setTextSize(2);
        tft.setCursor(10, yStart);
        tft.setTextColor(ST77XX_WHITE);
        tft.print("Main Menu");
        for (int i = 0; i < MAIN_MENU_ITEMS; ++i) {
            tft.setCursor(20, yStart + (i + 1) * 30);
            if (mainMenuSelectedIdx == i) {
                tft.setTextColor(ST77XX_BLACK, ST77XX_WHITE);
            } else {
                tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            }
            tft.print(menus[i]);
        }
        tft.setTextColor(ST77XX_WHITE);
    } else if (currentMenu == MENU_1) {
        // Draw title and options, highlight selected, scroll if needed
        const char* options[NUM_MENU1_OPTIONS] = {"Option 1", "Option 2", "Option 3", "Option 4"};
        int yStart = 10;
        int itemIdx = menu1ScrollIdx;
        int y = yStart;
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
    }
}