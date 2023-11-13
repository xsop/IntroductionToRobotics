/* 
Arduino stopwatch with lap memory
3 buttons representing:
    - start/pause
    - reset
    - lap
It uses interrupts for the start/pause and lap buttons.
The reset button is not interrupt because arduino uno has only 2 interrupt pins.
It uses a 4 digit 7 segment display to display the time.
*/

const int commonAnode = 0; // 1 for common anode, 0 for common cathode

const int encodingsNumber = 10;

byte lowDisplayValue = LOW;
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
byte decimalByteEncoding = B00000001;
byte ghostingFixByteEncoding = B00000000;

const int latchPin = 11;
const int clockPin = 10;
const int dataPin = 12;

const int startPauseButtonPin = 2; // interrupt pin
const int resetButtonPin = 8;
const int lapButtonPin = 3; // interrupt pin

const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;

int displayDigits[] = { segD1, segD2, segD3, segD4 };
const int displayCount = 4;

unsigned long lastIncrement = 0;
const unsigned long delayCount = 50;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

int timerMode = 0;

bool setTimer = false;
bool setPauseTimer = false;
unsigned long startTimer = 0;
unsigned long pauseTimer = 0;

byte lastStartPauseButtonState;
byte lastLapButtonState;

// calculated decimal point location
const unsigned int decimalPointMilliseconds = 2;
const unsigned int millisecondsToSecondsFactor = pow(10, decimalPointMilliseconds);

// timer modes, constants for better readability
// using just the number is also possible
const unsigned int idleMode = 0;
const unsigned int startMode = 1;
const unsigned int pauseMode = 2;
const unsigned int lapCycleMode = 3;

// outside index range is intended, so when reset is pressed it shows 000.0
const unsigned int lapMemorySize = 4;
int lapMemoryIndex = displayCount;
int lapNumber = 0;
int savedLaps[lapMemorySize];

int currentNumber;
int displayDigit;
int lastDigit;

void setup() {
    pinMode(startPauseButtonPin, INPUT_PULLUP);
    pinMode(resetButtonPin, INPUT_PULLUP);
    pinMode(lapButtonPin, INPUT_PULLUP);

	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);

    if(commonAnode) {
        invertByteEncodings();
    }

	for (int i = 0; i < displayCount; i++) {
		pinMode(displayDigits[i], OUTPUT);
		digitalWrite(displayDigits[i], lowDisplayValue);
	}

    attachInterrupt(digitalPinToInterrupt(startPauseButtonPin), startPauseButtonInterruptHandler, FALLING);
    attachInterrupt(digitalPinToInterrupt(lapButtonPin), lapButtonInterruptHandler, FALLING);
}

void loop() {
    getLastInterruptButtonsState();
    checkResetButton(); // non interrupt button because arduino uno has only 2 interrupt pins
    // also it's not needed to have extremely fast response time

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

byte invertByte(byte inputByte) {
    return ~inputByte;
}

void invertByteEncodings() {
    lowDisplayValue = invertByte(lowDisplayValue);
    for (int i = 0; i < encodingsNumber; i++) {
        byteEncodings[i] = invertByte(byteEncodings[i]);
    }

    decimalByteEncoding = invertByte(decimalByteEncoding);
    ghostingFixByteEncoding = invertByte(ghostingFixByteEncoding);
}

void getLastInterruptButtonsState() {
    lastStartPauseButtonState = digitalRead(startPauseButtonPin);
    lastLapButtonState = digitalRead(lapButtonPin);
}

void startPauseButtonInterruptHandler() {
    unsigned long currentMicros = micros();

    if (currentMicros - lastDebounceTime > debounceDelay * 1000 && lastStartPauseButtonState != digitalRead(startPauseButtonPin)) {
        lastDebounceTime = currentMicros;

        if (timerMode == idleMode || timerMode == pauseMode) {
            timerMode = startMode;
        } else if (timerMode == lapCycleMode) {
            goToZero();
            timerMode = startMode;
        } else if (timerMode == startMode) {
            timerMode = pauseMode;
        }
    }
}

void lapButtonInterruptHandler() {
    unsigned long currentMicros = micros();

    if (currentMicros - lastDebounceTime > debounceDelay * 1000 && lastLapButtonState != digitalRead(lapButtonPin)) {
        lastDebounceTime = currentMicros;

        if (timerMode == startMode) {
            pushLap((micros() / 1000 - startTimer) / millisecondsToSecondsFactor);
        }
        else if (timerMode == lapCycleMode) {
            if(lapMemoryIndex <= lapMemorySize - lapNumber || lapMemoryIndex <= 0) { 
                lapMemoryIndex = lapMemorySize - 1; 
            }
            else {
                lapMemoryIndex--;
            }
        }
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
        startTimer += millis() - pauseTimer; // adding the time that passed while paused
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
    if(lapMemoryIndex == displayCount) {
        writeNumber(0);
    }
    else {
        writeNumber(savedLaps[lapMemoryIndex]);
    }
}

void idleModeTimer() {
    goToZero();
    resetLaps();
}

void goToZero(){
    setPauseTimer = false;
    setTimer = false;
    lapMemoryIndex = displayCount;
    writeNumber(0);
}

void writeDigit(int digit, bool decimal = false) {
    digit += decimal * decimalByteEncoding; // adding a decimal point if needed
	digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, MSBFIRST, digit);
	digitalWrite(latchPin, HIGH);
}

void writeDigitToDisplayNoGhosting(int displayDigit, int digit, bool decimal = false) {
    activateDisplay(displayDigit);
    writeDigit(byteEncodings[digit], decimal);
    writeDigit(ghostingFixByteEncoding);
}

void writeNumber(int number) {
	currentNumber = number;
	displayDigit = displayCount - 1; // get the last digit from the display

    while(displayDigit >= 0) {
        if(currentNumber != 0) {
            lastDigit = currentNumber % 10;
            currentNumber /= 10;
        }
        else {
            lastDigit = 0; // if there are no more digits, print 0 to complete all the displays
        }
        writeDigitToDisplayNoGhosting(displayDigit, lastDigit, displayDigit == decimalPointMilliseconds);
        displayDigit--;
    }
}

void activateDisplay(int displayNumber) {
	for (int i = 0; i < displayCount; i++) {
		digitalWrite(displayDigits[i], !lowDisplayValue);
	}
	digitalWrite(displayDigits[displayNumber], lowDisplayValue);
}