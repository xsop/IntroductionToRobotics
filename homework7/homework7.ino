#include "Init.h"

void setup() {
    matrix.setupMatrix();

    matrixBrightness = EEPROM.read(matrixBrightnessAddress);
    if(matrixBrightness > 15 || matrixBrightness < 0){
        matrixBrightness = 15;
    }
    matrix.setBrightness(matrixBrightness);

    LCDBrightness = EEPROM.read(LCDBrightnessAddress);
    if(LCDBrightness > 255 || LCDBrightness < 0){
        LCDBrightness = 255;
    }
    display.setBrightness(map(LCDBrightness, 0, 255, minInput, maxInput));

    display.printIntro();
}

void loop() {
    if(isInGame){
        updateGame();
        display.printInGame();
    }
    else{
        if(isInGameOver){
            display.printGameOver();
            //bad practice, prevents clicking both exit and play
            while(controller.getJoystickButtonRead() == LOW){
                isInGameOver = false;
                isInMain = true;
                isInSettings = false;
                isInAbout = false;
                isInValueInput = false;
                cursorPos = 0;
                pagePos = 0;
                display.printMainMenu();
            }
        }
        else if(millis() > introDuration){
            controller.updateMenu();
        }
    }
}