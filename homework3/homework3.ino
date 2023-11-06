/* 
Code enables a "cursor" to move around a seven segment display 
with a joystick and toggle the segments with it's button.
Highlighted segment is blinking whether it is active or not.
If you hold the joystick button, all the segments will be reset.
*/

// preprocessing default low value for the segments
// if you want to use common anode displays
// change the value to 1
#define COMMON_ANODE 0
#if COMMON_ANODE
    const byte lowDisplayValue = HIGH;
#else
    const byte lowDisplayValue = LOW;
#endif

const int pinA = 12;
const int pinB = 10;
const int pinC = 9;
const int pinD = 8;
const int pinE = 7;
const int pinF = 6;
const int pinG = 5;
const int pinDP = 4;

const int noOfSegments = 8; // including the decimal point
const int defaultStartSegment = 7;
const int segmentPinsArray[noOfSegments] =  {pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP};

// declaring the segments it should go to if you move the joystick in a every direction
const int neighborSegment[noOfSegments][4] = {
    /*up, down, left, right
      u, d, l, r */
    { 0, 6, 5, 1 }, //a
    { 0, 6, 5, 1 }, //b
    { 6, 3, 4, 7 }, //c
    { 6, 3, 4, 2 }, //d
    { 6, 3, 4, 2 }, //e
    { 0, 6, 5, 1 }, //f
    { 0, 3, 6, 6 }, //g
    { 7, 7, 2, 7 } //dp
};

bool segmentStatesArray[noOfSegments] = {lowDisplayValue};
int currentSegment = defaultStartSegment;

//indexes for the neighborSegment array
//no direction could be any number that is not a valid index
//declaring them as constants for readability

const int noDirection = -1;
const int upDirection = 0;
const int downDirection = 1;
const int leftDirection = 2;
const int rightDirection = 3;
int direction = noDirection;

const int joystickPinX = A0;
const int joystickPinY = A1;
const int joystickPinSwitch = 2;
const int joystickMinCenterThreshold = 300;
const int joystickMaxCenterThreshold = 700;

bool joystickMoved = false;
int joystickValueX = 0;
int joystickValueY = 0;

const int blinkInterval = 500;
bool blinkState = false;
unsigned long lastBlink = 0;

const unsigned long debounceDelay = 200;
const unsigned long longPressDelay = 1000;
volatile bool buttonPressed = false;
volatile bool buttonLongPressed = false;
volatile unsigned long lastInterruptTime = 0;

void setup(){

    for(int i = 0; i < noOfSegments; i++){
        pinMode(segmentPinsArray[i], OUTPUT);
        digitalWrite(segmentPinsArray[i], segmentStatesArray[i]);
    }
    pinMode(joystickPinSwitch, INPUT_PULLUP);
    pinMode(joystickPinX, INPUT);
    pinMode(joystickPinY, INPUT);

    // interupt is done as per homework requirements
    // I think polling would be better in this case
    // since it has to check for both short and long presses
    attachInterrupt(digitalPinToInterrupt(joystickPinSwitch), handleInterrupt, CHANGE);
}

void loop(){
    readJoystickValues();

    // joystickMoved is a flag that makes sure you can do
    // only one move per joystick move from center

    if (joystickMoved == false) {
        direction = getDirection();
        if (direction != noDirection) {
            moveTo(direction);
        }
    } 
    else if (isJoystickInCenter()) {
        joystickMoved = false;
    }
    
    blinkCurrentSegment(); // the current segment is always blinking

    if(buttonPressed == true){
        toggleSegment(currentSegment);
    }
    if(buttonLongPressed == true){
        resetSegments();
    }
}

void handleInterrupt() {
    static unsigned long interruptTime = 0;
    interruptTime = micros();
    if(digitalRead(joystickPinSwitch) == LOW){
        if (interruptTime - lastInterruptTime > debounceDelay * 1000) {
            buttonPressed = true;
        }
    }
    else {
        if (interruptTime - lastInterruptTime > longPressDelay * 1000) {
            buttonLongPressed = true;
        }
    }
    lastInterruptTime = interruptTime;
}

void readJoystickValues(){
    joystickValueX = analogRead(joystickPinX);
    joystickValueY = analogRead(joystickPinY);
}

int getDirection(){
    if (joystickValueY < joystickMinCenterThreshold) {
        return upDirection;
    } else if (joystickValueY > joystickMaxCenterThreshold) {
        return downDirection;
    } else if (joystickValueX < joystickMinCenterThreshold) {
        return leftDirection;
    } else if (joystickValueX > joystickMaxCenterThreshold) {
        return rightDirection;
    }
    return noDirection;
}

void moveTo(int position){
    if(neighborSegment[currentSegment][position] == currentSegment){
        return;
    }

    // make sure the segment has the correct state and update/move the current segment
    digitalWrite(segmentPinsArray[currentSegment], segmentStatesArray[currentSegment]);
    currentSegment = neighborSegment[currentSegment][position];

    joystickMoved = true;

    // first blink is instant
    // because it gets and applies the opposite state immediately
    blinkState = !segmentStatesArray[currentSegment];
    lastBlink = millis();
    
}

bool isJoystickInCenter(){
    return (joystickValueX > joystickMinCenterThreshold && 
            joystickValueX < joystickMaxCenterThreshold && 
            joystickValueY > joystickMinCenterThreshold && 
            joystickValueY < joystickMaxCenterThreshold);
}

void blinkCurrentSegment(){
    if(millis() - lastBlink > blinkInterval){
        blinkState = !blinkState;
        lastBlink = millis();
    }
    digitalWrite(segmentPinsArray[currentSegment], blinkState);
}

void toggleSegment(int position){
    segmentStatesArray[position] = !segmentStatesArray[position];
    buttonPressed = false;
}

void resetSegments(){
    for(int i = 0; i < noOfSegments; i++){
        segmentStatesArray[i] = lowDisplayValue;
        digitalWrite(segmentPinsArray[i], lowDisplayValue);
    }
    currentSegment = defaultStartSegment;
    buttonLongPressed = false;
}