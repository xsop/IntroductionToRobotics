#pragma once

#include "Init.h"

class Entity {
public:
    Entity();
    void blink();
    byte getX() const { return x; }
    byte getY() const { return y; }

protected:
    byte x = playerStartX;
    byte y = playerStartY;

    bool visible = true;
    unsigned long blinkInterval;
    unsigned long lastBlink = 0;
};

class Player : public Entity {
public:
    Player();
    void movePlayer(byte x, byte y);
    bool isOutOfBounds(byte x, byte y) const;
};

class Bomb : public Entity {
public:
    Bomb();
    Bomb& operator=(const Bomb& other);
    bool timerRanOut() const { return (millis() - timePlaced) > bombTimer; }
    bool exploded();
private:
    unsigned long timePlaced;
    unsigned long explosionStart = 0;    
};

void blinkEntity(unsigned long &lastBlink, unsigned long blinkInterval, bool &visible, byte x, byte y);