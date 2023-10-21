#define DEBUG 0 // set to 1 to enable serial debug output
#if DEBUG
    const int serialBaudRate = 9600;
#endif

const int redResistance = A0;
const int greenResistance = A1;
const int blueResistance = A2;

const int redLed = 9;
const int greenLed = 10;
const int blueLed = 11;

const int maxResistanceValue = 1023;
const int maxLedValue = 255;
const int minResistanceValue = 0;
const int minLedValue = 0;

int redValue = 0;
int greenValue = 0;
int blueValue = 0;

void setup(){
    pinMode(redResistance, INPUT);
    pinMode(greenResistance, INPUT);
    pinMode(blueResistance, INPUT);

    pinMode(redLed, OUTPUT);
    pinMode(greenLed, OUTPUT);
    pinMode(blueLed, OUTPUT);

    #if DEBUG
        Serial.begin(serialBaudRate);
    #endif
}

void loop(){
    readLedValues();
    writeLedValues();

    #if DEBUG
        serialPrintResistanceValues();
    #endif
}

void readLedValues(){
    redValue = map(analogRead(redResistance), minResistanceValue, maxResistanceValue, minLedValue, maxLedValue);
    greenValue = map(analogRead(greenResistance), minResistanceValue, maxResistanceValue, minLedValue, maxLedValue);
    blueValue = map(analogRead(blueResistance), minResistanceValue, maxResistanceValue, minLedValue, maxLedValue);
}

void writeLedValues(){
    analogWrite(redLed, redValue);
    analogWrite(greenLed, greenValue);
    analogWrite(blueLed, blueValue);
}

#if DEBUG
    void serialPrintResistanceValues(){
        Serial.print("R: ");
        Serial.print(redValue);
        Serial.print(" G: ");
        Serial.print(greenValue);
        Serial.print(" B: ");
        Serial.print(blueValue);
        Serial.print("\n");
    }
#endif