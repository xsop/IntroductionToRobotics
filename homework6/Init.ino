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