#include "MenuManager.h"

MenuManager::MenuManager(Adafruit_ST7789& display) : tft(display) {}

void MenuManager::render() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print("Hello");
}