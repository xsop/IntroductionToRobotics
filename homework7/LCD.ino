#include "LCD.h"

Display::Display() {
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    analogWrite(displayLedPin, LCDBrightness);
}

void Display::printIntro() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Welcome to");
    lcd.setCursor(0, 1);
    lcd.print("BOMBERMAN");
}

char* mainMenuTitles[3] = {"Play", "Settings", "About"};

void Display::printMainMenu() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(mainMenuTitles[pagePos]);
    lcd.setCursor(0, 1);
    lcd.print(mainMenuTitles[pagePos + 1]);
}

char* settingsTitles[3] = {"LCD Brightness", "Matrix Brightness", "Back"};

void Display::printSettings() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(settingsTitles[pagePos]);
    lcd.setCursor(0, 1);
    lcd.print( settingsTitles[pagePos + 1]);
}

void Display::printAbout(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BOMBERMAN");
    lcd.setCursor(0, 1);
    lcd.print("github.com/xsop");
}

void Display::printValueInput(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter value:");
    lcd.setCursor(0, 1);
    lcd.print(currentValue);
}

void Display::printGameOver(){
    lcd.setCursor(0, 0);
    lcd.print("GAME OVER         ");
}

void Display::printInGame(){
    lcd.setCursor(0, 0);
    lcd.print("Dots left: ");
    lcd.print(matrix.getMemoryCounter());
    lcd.setCursor(0, 1);
    lcd.print("           ");
}   

void Display::setBrightness(int brightness){
    if(brightness < minInput){
        brightness = minInput;
    }
    else if(brightness > maxInput){
        brightness = maxInput;
    }
    LCDBrightness = map(brightness, minInput, maxInput, 0, 255);
    analogWrite(displayLedPin, LCDBrightness);
    EEPROM.update(LCDBrightnessAddress, LCDBrightness);
}

int Display::getBrightness(){
    return map(LCDBrightness, 0, 255, minInput, maxInput);
}