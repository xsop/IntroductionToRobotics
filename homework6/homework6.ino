#include "Init.h"

const int unusedPin = 5;

void setup() {
    Serial.begin(115200);

    matrix.setupMatrix();
    randomSeed(analogRead(unusedPin));
    gameMap.drawWalls();
}

void loop() {
    player.blink();
    controller.update();
    gameMap.update();
    
}