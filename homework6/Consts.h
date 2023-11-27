const int unusedPin = 5;

const byte dinPin = 12; // pin 12 is connected to the MAX7219 pin 1
const byte clockPin = 11; // pin 11 is connected to the CLK pin 13
const byte loadPin = 10; // pin 10 is connected to LOAD pin 12
const byte matrixSize = 8; // 1 as we are only using 1 MAX7219

const int playerStartX = 0;
const int playerStartY = 0;

const int bombBlinkInterval = 200;
const int playerBlinkInterval = 500;
const int collisionBlinkInterval = 100;

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
const int debounceDelay = 50;

//percentage not totally accurate
//it fills the map with walls until it reaches the percentage
//and removes walls in player spawn area
//if it spawns a wall on the player, it will be removed
//therefore the percentage will be lower
const int wallPercentage = 75;

const int matrixBrightness = 2;