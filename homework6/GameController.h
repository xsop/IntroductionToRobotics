#pragma once

#include "Init.h"

enum DIRECTION {
    NO_DIRECTION,
    UP_DIRECTION,
    DOWN_DIRECTION,
    LEFT_DIRECTION,
    RIGHT_DIRECTION
};

class Controller {
    public:
        Controller();
        void update();

        int getJoystickValueX() const { return joystickValueX; }
        int getJoystickValueY() const { return joystickValueY; }
        byte getJoystickButtonRead() const { return digitalRead(joystickPinSwitch); }

        void readJoystickValues();
        void handleButton();
        int getDirection();
        void movePlayer();
        bool isNextMoveAvailable();
        
    private:
        const int joystickPinX = A0;
        const int joystickPinY = A1;
        const int joystickPinSwitch = 2;
        const int joystickMinCenterThreshold = 300;
        const int joystickMaxCenterThreshold = 700;

        int joystickValueX = 0;
        int joystickValueY = 0;
        byte lastJoystickButtonState = 0;

        unsigned long lastMove = 0;
        unsigned long lastDebounceTime = 0;
        
        const int moveDelay = 100;
        const int debounceDelay = 50;
};
