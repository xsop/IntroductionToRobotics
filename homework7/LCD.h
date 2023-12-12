#pragma once

#include "Init.h"
#include "Consts.h"
#include "LiquidCrystal.h"

class Display {
public:
    Display();
    void printIntro();
    void printMainMenu();
    void printSettings();
    void printAbout();
    void printValueInput();
    void printGameOver();
    void printInGame();

    void setBrightness(int brightness);
    int getBrightness();

    LiquidCrystal lcd = LiquidCrystal(rs, en, d4, d5, d6, d7);
};