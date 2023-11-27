#pragma once

#include "Map.h"

class GameMap {
public:
    GameMap();
    void update();
    void generate();
    void placeBomb();
    bool isObstacle(int x, int y) const;

    int getMatrixSize() const { return matrixSize; }
    void setMatrixSize(int size) { matrixSize = size; }
private:
    int matrixSize;
    
};

Bomb* bomb = nullptr;

bool areEntitiesOnSameSpot();