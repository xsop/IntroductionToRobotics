#pragma once

const int unusedPin = 5;

const byte dinPin = 12; // pin 12 is connected to the MAX7219 pin 1
const byte clockPin = 11; // pin 11 is connected to the CLK pin 13
const byte loadPin = 10; // pin 10 is connected to LOAD pin 12
const byte matrixSize = 8; // 1 as we are only using 1 MAX7219

const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const byte displayLedPin = 3;

const int playerStartX = 0;
const int playerStartY = 0;

const int bombBlinkInterval = 200;
const int playerBlinkInterval = 500;
const int collisionBlinkInterval = 100;
const int introDuration = 1000;

const unsigned long bombTimer = 3000;
const int explosionRadius = 2;
const unsigned long animationDuration = 500;
const unsigned long fullMatrixOnDelay = 1000;

const int joystickPinX = A0;
const int joystickPinY = A1;
const int joystickPinSwitch = 2;
const int joystickMinCenterThreshold = 300;
const int joystickMaxCenterThreshold = 700;

const int moveDelay = 100;
const int menuDelay = 500;
const int debounceDelay = 50;

//percentage not totally accurate
//it fills the map with walls until it reaches the percentage
//and removes walls in player spawn area
//if it spawns a wall on the player, it will be removed
//therefore the percentage will be lower
const int wallPercentage = 70;

int matrixBrightness = 2;
int LCDBrightness = 255;

const byte minInput = 0;
const byte maxInput = 15;

const int matrixBrightnessAddress = 321;
const int LCDBrightnessAddress = 896;