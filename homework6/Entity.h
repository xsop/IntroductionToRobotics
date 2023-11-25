#pragma once

#include "Init.h"

class Entity {
public:
    Entity();
    void blink();
    byte getX() const { return x; }
    byte getY() const { return y; }

protected:
    byte x = 0;
    byte y = 0;

    bool visible = true;
    int blinkInterval;
    unsigned long lastBlink = 0;
};

class Player : public Entity {
public:
    Player();
    void movePlayer(byte x, byte y);
    int isOutOfBounds(int x, int y) const;
};

class Bomb : public Entity {
public:
    Bomb();
    Bomb& operator=(const Bomb& other);
    bool timerRanOut() const { return (millis() - timePlaced) > timer; }
    bool explode();
private:
    unsigned long timer = 3000;
    unsigned long timePlaced;
    int explosionRadius = 2;
    unsigned long explosionStart = 0;
    int animationDuration = 500;
    
};
