#include "GameController.h"

Controller::Controller() {
    pinMode(joystickPinX, INPUT);
    pinMode(joystickPinY, INPUT);
    pinMode(joystickPinSwitch, INPUT_PULLUP);
}

void Controller::update() {
    storeJoystickValues();
    if(isNextMoveAvailable()) {
        movePlayer();
    }
    handleButton();
}

void Controller::storeJoystickValues(){
    joystickValueX = analogRead(joystickPinX);
    joystickValueY = analogRead(joystickPinY);
}

void Controller::handleButton(){
    if(getJoystickButtonRead() != lastJoystickButtonState) {
        if(millis() - lastDebounceTime > debounceDelay) {
            if(getJoystickButtonRead() == LOW) {  
                gameMap.placeBomb();
            }
        }
        lastDebounceTime = millis();
    }

    lastJoystickButtonState = getJoystickButtonRead();
}

int Controller::getDirection() const{
    if (getJoystickValueY() < joystickMinCenterThreshold) {
        return UP_DIRECTION;
    } else if (getJoystickValueY() > joystickMaxCenterThreshold) {
        return DOWN_DIRECTION;
    } else if (getJoystickValueX() < joystickMinCenterThreshold) {
        return LEFT_DIRECTION;
    } else if (getJoystickValueX() > joystickMaxCenterThreshold) {
        return RIGHT_DIRECTION;
    }
    return NO_DIRECTION;
}

void Controller::movePlayer(){
    int direction = getDirection();
    switch(direction) {
        case UP_DIRECTION:
            player.movePlayer(player.getX(), player.getY() - 1);
            lastMove = millis();
            break;
        case DOWN_DIRECTION:
            player.movePlayer(player.getX(), player.getY() + 1);
            lastMove = millis();
            break;
        case LEFT_DIRECTION:
            player.movePlayer(player.getX() - 1, player.getY());
            lastMove = millis();
            break;
        case RIGHT_DIRECTION:
            player.movePlayer(player.getX() + 1, player.getY());
            lastMove = millis();
            break;
        default:
            break;
    }
}

bool Controller::isNextMoveAvailable() const{
    if(millis() - lastMove > moveDelay) {
        return true;
    }
    return false;
}