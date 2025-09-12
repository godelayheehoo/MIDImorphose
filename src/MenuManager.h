#pragma once
#include <Adafruit_ST7789.h>

class MenuManager {
public:
    MenuManager(Adafruit_ST7789& display);
    void render();
private:
    Adafruit_ST7789& tft;
};