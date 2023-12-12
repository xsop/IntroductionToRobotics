#include "GameController.h"

Controller::Controller() {
    pinMode(joystickPinX, INPUT);
    pinMode(joystickPinY, INPUT);
    pinMode(joystickPinSwitch, INPUT_PULLUP);
}

void Controller::update() {
    storeJoystickValues();
    if(isNextMoveAvailable(moveDelay)) {
        movePlayer();
    }
    handleButton();
}


void Controller::updateMenu() {
    display.lcd.setCursor(0, cursorPos);
    display.lcd.blink();
    storeJoystickValues();
    if(isNextMoveAvailable(menuDelay)) {
        moveMenu();
    }
    if(getJoystickButtonRead() != lastJoystickButtonState) {
        if(millis() - lastDebounceTime > debounceDelay) {
            if(getJoystickButtonRead() == LOW) {  
                menu();
            }
        }
        lastDebounceTime = millis();
    }

    lastJoystickButtonState = getJoystickButtonRead();
    
    if(changePrint){

        if(isInMain){
            display.printMainMenu();
        }
        if(isInSettings){
            display.printSettings();
        }
        if(isInAbout){
            display.printAbout();
        }
        if(isInValueInput){
            display.printValueInput();
        }

        changePrint = false;
    }

    if(isInValueInput){
        if(isNextMoveAvailable(menuDelay)){
            if(getDirection() == RIGHT_DIRECTION){
                if(currentValue < maxInput){
                    currentValue++;
                    changePrint = true;
                }
            
            }
            else if(getDirection() == LEFT_DIRECTION){
                if(currentValue > minInput){
                    currentValue--;
                    changePrint = true;
                }
            }
            lastMove = millis();
        }
    }
}

void Controller::moveMenu(){
    int direction = getDirection();
    switch (direction) {
    case UP_DIRECTION:
        if(cursorPos == 1){
            cursorPos = 0;
            lastMove = millis();
        }
        else if(pagePos == 1){
            pagePos = 0;
            lastMove = millis();
            changePrint = true;
        }
        break;

    case DOWN_DIRECTION:
        if(cursorPos == 0){
            cursorPos = 1;
            lastMove = millis();
        }
        else if(pagePos == 0){
            pagePos = 1;
            lastMove = millis();
            changePrint = true;
        }
        break;

    default:
        break;
    }
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

bool Controller::isNextMoveAvailable(int delay) const{
    if(millis() - lastMove > delay) {
        return true;
    }
    return false;
}