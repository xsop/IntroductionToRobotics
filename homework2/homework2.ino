const int buzzerPin = 12;
const int readyLedPin = 13;

const int numFloors = 3;
const int buttonPins[] = {2,3,4};
const int floorLedPins[] = {5,6,7};

const unsigned long timeToCloseDoors = 2000;
const unsigned long timeToMove = 1000;
const unsigned long timeToBlinkLed = 300;

const int noteDuration = 1000;
const int notes[] = {
    // Notes gathered from https://www.arduino.cc/en/Tutorial/BuiltInExamples/toneMelody
    // and kept only the ones that are used for the buzzer
    82, // NOTE_E2
    55, // NOTE_A1
    294 // NOTE_D4
};

bool statusClosedDoors = 0;
bool statusInit = 0;
bool statusFloor = 1;
bool statusReadyLedState = 1;

int currentFloor = 0;
int targetFloor = 0;
int direction = 0;

unsigned long lastMove = 0;
unsigned long lastInit = 0;
unsigned long lastReadyLedBlink = 0;
unsigned long lastSoundPlay = 0;

void setup() {
    pinMode(readyLedPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    for(int i = 0; i < numFloors; i++){
        pinMode(buttonPins[i], INPUT_PULLUP);
        pinMode(floorLedPins[i], OUTPUT);
    }
    digitalWrite(floorLedPins[currentFloor], HIGH);
    digitalWrite(readyLedPin, HIGH);
}

void loop() {
    if(currentFloor == targetFloor){
        stationaryElevator();
        if(isSoundFinished())
            checkInputFloor();
    }
    else{
        // get the millis() value when the elevator should go to the target floor
        // only once and it is used to calculate the time to close the doors
        if(!statusInit)     
            initTimer(); 

        if(!statusClosedDoors)
            closeDoors();
        else
            moveElevator();
    }
}

void stationaryElevator(){

    // resets elevator status and opens doors

    if(statusClosedDoors){
        playSound(1, 2);
        lastSoundPlay = millis();
        statusClosedDoors = 0;
    }
    digitalWrite(readyLedPin, HIGH);
    statusReadyLedState = 1;
    statusInit = 0;
}

void playSound(bool isNoTone, int indSound){
    if(isNoTone)
        noTone(buzzerPin);
    tone(buzzerPin, notes[indSound], noteDuration);
}

int isSoundFinished(){
    return (millis() - lastSoundPlay > noteDuration) || (lastSoundPlay == 0);
}

void checkInputFloor(){
    for(int i = 0; i < numFloors; i++){
        if(digitalRead(buttonPins[i]) == LOW){
            targetFloor = i;
            break;
        }
    }
}

void initTimer(){
    lastInit = millis();
    playSound(1, 1);
    statusInit = 1;
}

void closeDoors(){
    if(millis() - lastInit > timeToCloseDoors){
        digitalWrite(floorLedPins[targetFloor], LOW);
        lastMove = millis();
        statusClosedDoors = 1;
    }
}

void moveElevator(){
    blinkReadyLed();
    playSound(0, 0);

    if(millis() - lastMove > timeToMove){
        lastMove = millis();
        direction = (currentFloor < targetFloor) ? 1 : -1;

        // statusFloor represents the current state of the floor led
        // if statusFloor is 0 -> floor led is on -> it is at a floor
        // if statusFloor is 1 -> floor led is off -> it is between floors

        if(statusFloor){
            digitalWrite(floorLedPins[currentFloor], LOW);
        }
        else{
            currentFloor += direction;
            digitalWrite(floorLedPins[currentFloor], HIGH);
        }

        statusFloor = !statusFloor;
    }
}

void blinkReadyLed(){
    if(millis() - lastReadyLedBlink > timeToBlinkLed){
        lastReadyLedBlink = millis();
        statusReadyLedState = !statusReadyLedState;
        digitalWrite(readyLedPin, statusReadyLedState);
    }
}