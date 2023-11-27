#include "Init.h"

void setup() {
    matrix.setupMatrix();
    randomSeed(analogRead(unusedPin));
    gameMap.generate();
}

void loop() {
    player.blink();
    controller.update();
    gameMap.update();
}