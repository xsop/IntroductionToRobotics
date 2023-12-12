#include "Init.h"

void fullMatrixOn() {
    for(int i = 0; i < matrix.getMatrixSize(); i++) {
        for(int j = 0; j < matrix.getMatrixSize(); j++) {
            matrix.setLed(i, j, true, false);
        }
    }
}

void startGame() {
    //make the whole matrix red with delay
    fullMatrixOn();
    delay(fullMatrixOnDelay);
    
    //putting everything back to the initial state
    player.movePlayer(playerStartX, playerStartY);

    matrix.setupMatrix();
    // regenerate random seed
    // needed to make a new game map
    randomSeed(analogRead(unusedPin));
    gameMap.generate();
}

void updateGame() {
    player.blink();
    controller.update();
    gameMap.update();
}

void menu(){
    byte index = cursorPos + pagePos;
    if(isInMain){
        if(index == 0){
            startGame();
            isInGame = true;
        }
        if(index == 1){
            changePrint = true;
            isInMain = false;
            isInSettings = true;
            isInAbout = false;
            isInValueInput = false;
            cursorPos = 0;
            pagePos = 0;
        }
        else if(index == 2){
            changePrint = true;
            isInMain = false;
            isInSettings = false;
            isInAbout = true;
            isInValueInput = false;
            cursorPos = 0;
            pagePos = 0;
        }
    }
    else if(isInSettings){
        if(index == 0 || index == 1){
            if(index == 0){
                currentValue = display.getBrightness();
            }
            else if(index == 1){
                currentValue = matrix.getBrightness();
            }
            changePrint = true;
            isInMain = false;
            isInSettings = false;
            isInAbout = false;
            isInValueInput = true;
        }
        else if(index == 2){
            changePrint = true;
            isInMain = true;
            isInSettings = false;
            isInAbout = false;
            isInValueInput = false;
            cursorPos = 0;
            pagePos = 0;
        }
    }
    else if(isInAbout){
        changePrint = true;
        isInMain = true;
        isInSettings = false;
        isInAbout = false;
        isInValueInput = false;
        cursorPos = 0;
        pagePos = 0;
    }
    else if(isInValueInput){
        changePrint = true;
        isInMain = false;
        isInSettings = true;
        isInAbout = false;
        isInValueInput = false;
        if(index == 0){
            display.setBrightness(currentValue);
        }
        else if(index == 1){
            matrix.setBrightness(currentValue);
        }
        cursorPos = 0;
        pagePos = 0;
    }
}