#pragma once

#include "Map.h"

class GameMap {
public:
    GameMap();
    void update();
    void drawWalls();
    void placeBomb();
    bool isWall(int x, int y) const;

    int getMatrixSize() const { return matrixSize; }
    void setMatrixSize(int size) { matrixSize = size; }
private:
    int matrixSize;
    const int wallPercentage = 50;
};

Bomb* bomb = nullptr;
