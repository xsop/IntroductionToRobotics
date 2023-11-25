#include "Init.h"

GameMap::GameMap() {
    setMatrixSize(matrix.getMatrixSize());
}

void GameMap::update() {
    if(bomb != nullptr) {
        if(bomb->timerRanOut()) {
            if(bomb->explode()){
                delete bomb;
                bomb = nullptr;
                return;
            }
        }
        bomb->blink();
    }
}

void GameMap::drawWalls() {
    int wallCount = (matrixSize * matrixSize) * wallPercentage / 100;
    int wallX = 0;
    int wallY = 0;
    for(int i = 0; i < wallCount; i++) {
        wallX = random(0, matrixSize);
        wallY = random(0, matrixSize);
        if(abs(wallX - player.getX()) < 2 && abs(wallY - player.getY()) < 2) {
            continue;
        }
        matrix.setLed(wallX, wallY, true);
    }
}

bool GameMap::isWall(int x, int y) const {
    return matrix.getLed(x, y);
}

void GameMap::placeBomb() {
    bomb = new Bomb();
}
