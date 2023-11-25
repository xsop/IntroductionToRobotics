#include "Entity.h"

Entity::Entity() {

}

void Entity::blink() {
    if(millis() - lastBlink > blinkInterval) {
        lastBlink = millis();
        
        matrix.setLed(x, y, visible, false);
        visible = !visible;
    }
}

Player::Player() {
    visible = true;
    blinkInterval = 500;
}

void Player::movePlayer(byte x, byte y) {
    if(this->x == x && this->y == y) {
        return;
    }
    if(isOutOfBounds(x, y)) {
        return;
    }
    if(gameMap.isWall(x, y)) {
        return;
    }
    matrix.setLed(this->x, this->y, matrix.getLed(this->x, this->y), true);

    this->x = x;
    this->y = y;

    matrix.setLed(this->x, this->y, true, false);
    
    lastBlink = millis(); // reset blink timer for consistent first blink
}

int Player::isOutOfBounds(int x, int y) const {
    int matrixSize = matrix.getMatrixSize();
    if(x < 0 || x > matrixSize - 1 || y < 0 || y > matrixSize - 1) {
        return 1;
    }
    return 0;
}

Bomb::Bomb() {
    visible = true;
    blinkInterval = 200;
    timePlaced = millis();
    x = player.getX();
    y = player.getY();
}

Bomb& Bomb::operator=(const Bomb& other) {
    x = other.x;
    y = other.y;
    visible = other.visible;
    blinkInterval = other.blinkInterval;
    lastBlink = other.lastBlink;
    return *this;
}

bool Bomb::explode() {
    if(explosionStart == 0){
        for(int i = 0; i < explosionRadius; i++) {
            matrix.setLed(x + i, y, true, false);
            matrix.setLed(x - i, y, true, false);
            matrix.setLed(x, y + i, true, false);
            matrix.setLed(x, y - i, true, false);
            explosionStart = millis();
        }
        explosionRadius -= 1; // too lazy to do math
        if((player.getX() <= x + explosionRadius && player.getX() >= x - explosionRadius && player.getY() == y) ||
            (player.getY() <= y + explosionRadius && player.getY() >= y - explosionRadius && player.getX() == x)){
            //reset game
            //todo beautify and remove magic numbers
            //make the whole matrix red
            for(int i = 0; i < matrix.getMatrixSize(); i++) {
                for(int j = 0; j < matrix.getMatrixSize(); j++) {
                    matrix.setLed(i, j, true, false);
                }
            }
            delay(1000);
            player.movePlayer(0, 0);
            matrix.setupMatrix();
            randomSeed(analogRead(5));
            gameMap.drawWalls();
            
        }
    }
    else if(millis() - explosionStart > animationDuration) {
        explosionRadius += 1;
        for(int i = 0; i < explosionRadius; i++) {
            matrix.setLed(x + i, y, false, true);
            matrix.setLed(x - i, y, false, true);
            matrix.setLed(x, y + i, false, true);
            matrix.setLed(x, y - i, false, true);
        }
        explosionStart = 0;
        return 1;
    }
    return 0;
}