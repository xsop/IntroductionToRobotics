const int latchPin = 11;
const int clockPin = 10;
const int dataPin = 12;

const int startPauseButtonPin = 2;
const int resetButtonPin = 8;
const int lapButtonPin = 3;

const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;

int displayDigits[] = { segD1, segD2, segD3, segD4 };
const int displayCount = 4;

const int encodingsNumber = 10;

byte byteEncodings[encodingsNumber] = {
	//A B C D E F G DP
	B11111100,	// 0
	B01100000,	// 1
	B11011010,	// 2
	B11110010,	// 3
	B01100110,	// 4
	B10110110,	// 5
	B10111110,	// 6
	B11100000,	// 7
	B11111110,	// 8
	B11110110,	// 9
};
unsigned long lastIncrement = 0;
unsigned long delayCount = 50;
unsigned long number = 0;

bool setTimer = false;
bool setPauseTimer = false;
unsigned long startTimer = 0;
unsigned long pauseTimer = 0;

int timerMode = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 200;
byte lastStartPauseButtonState;
byte lastLapButtonState;

const unsigned int decimalPointMilliseconds = 2;
const unsigned int millisecondsToSecondsFactor = pow(10, decimalPointMilliseconds);

const unsigned int idleMode = 0;
const unsigned int startMode = 1;
const unsigned int pauseMode = 2;
const unsigned int lapCycleMode = 3;

// outside index range is intended, so when reset is pressed it shows 000.0 (as homework requires)
// it is outside so that it won't cycle back to 000.0 through normal lap time cycling
// I think that this shouldn't be intended but it's what the homework asks for
int index = displayCount;
int lapNumber = 0;
int savedLaps[displayCount];

const unsigned int lapMemorySize = 4;

void setup() {
    pinMode(startPauseButtonPin, INPUT_PULLUP);
    pinMode(resetButtonPin, INPUT_PULLUP);
    pinMode(lapButtonPin, INPUT_PULLUP);

	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);

	for (int i = 0; i < displayCount; i++) {
		pinMode(displayDigits[i], OUTPUT);
		digitalWrite(displayDigits[i], LOW);
	}

    attachInterrupt(digitalPinToInterrupt(startPauseButtonPin), startPauseButtonPressed, FALLING);
    attachInterrupt(digitalPinToInterrupt(lapButtonPin), lapButtonPressed, FALLING);
}

void loop() {
    getLastInterruptButtonsState();
    checkResetButton();

    switch(timerMode) {
        case startMode:
            startModeTimer();
            break;
        case pauseMode:
            pauseModeTimer();
            break;
        case lapCycleMode:
            lapCycleModeTimer();
            break;
        default:
            idleModeTimer();
            break;
    }
}

void getLastInterruptButtonsState() {
    lastStartPauseButtonState = digitalRead(startPauseButtonPin);
    lastLapButtonState = digitalRead(lapButtonPin);
}

void pushLap(int lap) {
    for (int i = 0; i < lapMemorySize - 1; i++) {
        savedLaps[i] = savedLaps[i + 1];
    }
    savedLaps[lapMemorySize - 1] = lap;
    incrementLapNumber();
}

void incrementLapNumber(){
    if(!(lapNumber + 1 > lapMemorySize)) {
        lapNumber++;
    }
}

void resetLaps() {
    for (int i = 0; i < lapMemorySize; i++) {
        savedLaps[i] = 0;
    }
    lapNumber = 0;

}

void startModeTimer() {
    if(setPauseTimer){
        startTimer += millis() - pauseTimer;
        setPauseTimer = false;
    }

    if(!setTimer) {
        startTimer = millis();
        setTimer = true;
    }

    writeNumber((millis() - startTimer) / millisecondsToSecondsFactor);
}

void pauseModeTimer() {
    if(!setPauseTimer) {
        pauseTimer = millis();
        setPauseTimer = true;
    }
    writeNumber((pauseTimer - startTimer) / millisecondsToSecondsFactor);
}

void lapCycleModeTimer() {
    if(index == displayCount) {
        writeNumber(0);
    }
    else {
        writeNumber(savedLaps[index]);
    }
}

void idleModeTimer() {
    setPauseTimer = false;
    setTimer = false;
    index = displayCount;
    resetLaps();
    writeNumber(0);
}

void startPauseButtonPressed() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastDebounceTime > debounceDelay && lastStartPauseButtonState != digitalRead(startPauseButtonPin)) {
        lastDebounceTime = currentMillis;

        if (timerMode == idleMode || timerMode == pauseMode) {
            timerMode = startMode;
        } else if (timerMode == startMode) {
            timerMode = pauseMode;
        }
    }
}

void lapButtonPressed() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastDebounceTime > debounceDelay && lastLapButtonState != digitalRead(lapButtonPin)) {
        lastDebounceTime = currentMillis;

        if (timerMode == startMode) {
            pushLap((millis() - startTimer) / millisecondsToSecondsFactor);
        }
        else if (timerMode == lapCycleMode) {
            if(index <= lapMemorySize - lapNumber || index <= 0) { 
                index = lapMemorySize - 1; 
            }
            else {
                index--;
            }
        }
    }
}

void writeDigit(int digit, bool decimal = false) {
    digit += decimal * B00000001;
	digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, MSBFIRST, digit);
	digitalWrite(latchPin, HIGH);
}

void writeDigitToDisplayNoGhosting(int displayDigit, int digit, bool decimal = false) {
    activateDisplay(displayDigit);
    writeDigit(byteEncodings[digit], decimal);
    writeDigit(B00000000);
}

void activateDisplay(int displayNumber) {
	for (int i = 0; i < displayCount; i++) {
		digitalWrite(displayDigits[i], HIGH);
	}
	digitalWrite(displayDigits[displayNumber], LOW);
}

void writeNumber(int number) {
	int currentNumber = number;
	int displayDigit = 3;
	int lastDigit = 0;

    while(displayDigit >= 0) {
        if(currentNumber != 0) {
            lastDigit = currentNumber % 10;
            currentNumber /= 10;
        }
        else {
            lastDigit = 0;
        }
        writeDigitToDisplayNoGhosting(displayDigit, lastDigit, displayDigit == decimalPointMilliseconds);
        displayDigit--;
    }
}

void checkResetButton() {
    static bool resetButtonPressed = false;
    if(!resetButtonPressed && millis() - lastDebounceTime > debounceDelay && digitalRead(resetButtonPin) == LOW) {
        if(timerMode == lapCycleMode) {
            timerMode = idleMode;
        }
        else if(timerMode == pauseMode){
            timerMode = lapCycleMode;
        }
        
        lastDebounceTime = millis();
        resetButtonPressed = true;
    }
    else if (digitalRead(resetButtonPin) == HIGH) {
        resetButtonPressed = false;
    }
}