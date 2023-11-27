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

        int getDirection() const;

        void storeJoystickValues();
        void handleButton();

        void movePlayer();

        bool isNextMoveAvailable() const;
        
        
    private:
        int joystickValueX = 0;
        int joystickValueY = 0;
        byte lastJoystickButtonState = 0;

        unsigned long lastMove = 0;
        unsigned long lastDebounceTime = 0;
};
