// Smart Environment Monitor and Logger

// This program gathers data from an ultrasonic sensor and a light dependent resistor
// and logs the data in the EEPROM memory
// Also uses the EEPROM memory to store settings such as sampling interval and alert thresholds
// The data can be displayed on the serial monitor
// The program also has a menu system to change the behavior of the program

#include <EEPROM.h>

// CAUTION: EEPROM memory has a limited life of 100,000 write/erase cycles.
// Ideally, you don't reset the EEPROM
// Also the values should be validated at launch so that they are within the expected range

// But if default values are making the code work unexpectedly
// set RESET_EEPROM to 1 to reset the EEPROM 
// don't forget to set it back to 0 after resetting the EEPROM
#define RESET_EEPROM 0

// optional variable function declarations
void setSetting(int min, int max, int address, char settingPrompt[], char unit[], bool convertToSeconds = false);
void logValue(int value, int multiplier = 0);

const int defaultEEPROMValue = 0xFF;

const char exitOptionChar = 'q';
const char invalidInput[] = "Invalid Input";

const int trigPin = 9;
const int echoPin = 10;

const int numLEDColors = 3;
const int ledColorPins[numLEDColors] = {3, 5 ,6}; // PWM pins

const int ldrPin = A0;

const int minSamplingInterval = 1; //seconds
const int maxSamplingInterval = 10;

const int minDistanceAlertThreshold = 0;
const int maxDistanceAlertThreshold = 100;

const int minLDRAlertThreshold = 0;
const int maxLDRAlertThreshold = 100;

const int minRGBValue = 0;
const int maxRGBValue = 255;

const int minLDRReading = 0;
const int maxLDRReading = 1023;

// not necessary to have all these enums, also I think it makes the code harder to modify
// but it makes easier to read the input check functions

enum OPTIONS {NOT_OPTION, PRINT_SENSOR_READINGS, SET_SAMPLING_INTERVAL, SET_DISTANCE_ALERT_THRESHOLD, SET_LDR_ALERT_THRESHOLD, MANUAL_COLOR_CONTROL};
OPTIONS option = NOT_OPTION;

enum SUBMENUS {NOT_SUBMENU, SENSOR_SETTINGS, RESET_LOGGER_DATA, SYSTEM_STATUS, RGB_LED_CONTROL};
SUBMENUS subMenu = NOT_SUBMENU;

enum SENSOR_SETTINGS_INPUTS {NOT_SENSOR_SETTINGS_INPUT, SAMPLING_INTERVAL, DISTANCE_ALERT_THRESHOLD, LDR_ALERT_THRESHOLD, SENSOR_SETTINGS_EXIT};
SENSOR_SETTINGS_INPUTS sensorSettingsInput = NOT_SENSOR_SETTINGS_INPUT;

enum RESET_LOGGER_DATA_INPUTS {NOT_RESET_LOGGER_DATA_INPUT, YES, NO};
RESET_LOGGER_DATA_INPUTS resetLoggerDataInput = NOT_RESET_LOGGER_DATA_INPUT;

enum SYSTEM_STATUS_INPUTS {NOT_SYSTEM_STATUS_INPUTS, CURRENT_SENSOR_READINGS, CURRENT_SENSOR_SETTINGS, DISPLAY_LOGGED_DATA, SYSTEM_STATUS_EXIT};
SYSTEM_STATUS_INPUTS systemStatusInput = NOT_SYSTEM_STATUS_INPUTS;

enum RGB_LED_CONTROL_INPUTS {NOT_RGB_LED_CONTROL_INPUTS, MANUAL_COLOR_CONTROL_INPUT, TOGGLE_AUTOMATIC_RGB, RGB_LED_CONTROL_EXIT};
RGB_LED_CONTROL_INPUTS rgbLEDControlInput = NOT_RGB_LED_CONTROL_INPUTS;

// keyboard input
const int defaultInput = 0;
int mainMenuInput = defaultInput; 
int subMenuInput = defaultInput;

int prevLogIndex = 0;
int logIndex = 0;
const int numSensorLogs = 10;
const int numSensors = 2;
const int logValueSize = sizeof(int);
unsigned long lastLoggedTime = 0;

// logging memory will be organized as
// d d ... d l l ... l
// where d is the distance reading and l is the light reading
// the order the values are logged is circular to minimize the number of writes to the EEPROM
// also made it take account of multiple bytes per value to make it more flexible
// in this case logValueSize has the size of int (2 bytes)
// in case of EEPROM corruption it is easy to offset the whole memory by adding the number of bytes offset
const int samplingIntervalAddress = numSensorLogs * logValueSize * numSensors;

// eeprom addresses for settings will be after the logging memory
const int distanceAlertThresholdAddress = samplingIntervalAddress + logValueSize;
const int ldrAlertThresholdAddress = distanceAlertThresholdAddress + logValueSize;
const int isAutomaticRGBAddress = ldrAlertThresholdAddress + logValueSize;

// will be used as the base address for the RGB LED values
// one byte since I will use (r, g, b) format => 0-255
// +1 = green
// +2 = blue
const int redLEDAddress = isAutomaticRGBAddress + logValueSize; 

void setup() {

    #if RESET_EEPROM
        resetEEPROM();
    #endif

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    for(int i = 0; i < numLEDColors; i++){
        pinMode(ledColorPins[i], OUTPUT);
    }

    Serial.begin(9600);
    verifyEEPROM();
    printMainMenu();
    
}

void loop() {
    saveSensorLog();
    setLED();

    if(option == NOT_OPTION){
        if(subMenu == NOT_SUBMENU){
            checkMainMenuInput(); // if it's not within a submenu or option, it's in the main menu
        }   
        else{
            // if it's not within an option, it's in a submenu
            // go to the specific submenu's input checking function
            checkSubMenuInput(); 
        }
    } else {
        // if it's an option, go to the specific option's input checking function
        // in case of options which don't require input, it won't reach this part
        // the option would do the necessary actions and return to the main menu
        checkOptionInput(); 
    }
}

#if RESET_EEPROM
    void resetEEPROM(){
        for(int i = 0; i < EEPROM.length(); i++){
            EEPROM.write(i, 0xFF);
        }
    }
#endif

int intEEPROMGet(int index){
    // EEPROM.get() but it returns the int instead of being passed by argument
    // useful so there's no need to create a variable every time
    int value;
    EEPROM.get(index, value);
    return value;
}

void verifyEEPROM() {
    // useful to mitigate the risk of the EEPROM passing values in the wrong range
    // expecially useful for setting the sampling interval since a very small sampling interval
    // could cause the Arduino to corrupt the EEPROM by writing too often
    if(intEEPROMGet(samplingIntervalAddress) <= minSamplingInterval * 1000 
    || intEEPROMGet(samplingIntervalAddress) >= maxSamplingInterval * 1000){
        EEPROM.put(samplingIntervalAddress, maxSamplingInterval * 1000);
    }

    if(intEEPROMGet(distanceAlertThresholdAddress) <= minDistanceAlertThreshold
    || intEEPROMGet(distanceAlertThresholdAddress) >= maxDistanceAlertThreshold){
        EEPROM.put(distanceAlertThresholdAddress, maxDistanceAlertThreshold);
    }

    if(intEEPROMGet(ldrAlertThresholdAddress) <= minLDRAlertThreshold 
    || intEEPROMGet(ldrAlertThresholdAddress) >= maxLDRAlertThreshold){
        EEPROM.put(ldrAlertThresholdAddress, maxLDRAlertThreshold);
    }

    if(intEEPROMGet(isAutomaticRGBAddress) != 0 && intEEPROMGet(isAutomaticRGBAddress) != 1){
        EEPROM.put(isAutomaticRGBAddress, 1);
    }

}

void saveSensorLog(){
    if(millis() - lastLoggedTime > intEEPROMGet(samplingIntervalAddress)){
        lastLoggedTime = millis();

        logValue(getUltrasonicReading()); // put in the first range of the logging memory
        logValue(getLDRReading(), 1); // put in the second range of the logging memory

        if(option == PRINT_SENSOR_READINGS){
            printIndexLoggedData(prevLogIndex % (numSensorLogs * logValueSize));
        }
    }
    
}

void logValue(int value, int multiplier = 0){
    EEPROM.put(logIndex + multiplier * numSensorLogs * logValueSize, value);
    if(multiplier == 1){
        // needed for readings 
        prevLogIndex = logIndex;

        // circle back to first value if it reaches the end
        logIndex = (logIndex + logValueSize) % (numSensorLogs * logValueSize); 
    }
        
}

int getUltrasonicReading(){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    return pulseIn(echoPin, HIGH) * 0.034/2;
}

int getLDRReading(){
    return map(analogRead(ldrPin), minLDRReading, maxLDRReading, minLDRAlertThreshold, maxLDRAlertThreshold); // convert to percentage
}

void setLED(){
    if(intEEPROMGet(isAutomaticRGBAddress)){
        if(isOutOfRange(intEEPROMGet(prevLogIndex), intEEPROMGet(distanceAlertThresholdAddress)) ||
           isOutOfRange(intEEPROMGet(prevLogIndex + numSensorLogs * logValueSize), intEEPROMGet(ldrAlertThresholdAddress))){
            setRGBLEDColor(maxRGBValue, minRGBValue, minRGBValue); //red
        }
        else {
            setRGBLEDColor(minRGBValue, maxRGBValue, minRGBValue); //green
        }
    }
    else {
        setRGBLEDColor(EEPROM.read(redLEDAddress), EEPROM.read(redLEDAddress + 1), EEPROM.read(redLEDAddress + 2));
    }
}

bool isOutOfRange(int sensorReading, int sensorAlertThreshold){
    // can be modified to be min or max threshold
    // in this case, min threshold
    return sensorReading < sensorAlertThreshold;
}

void setRGBLEDColor(int red, int green, int blue){
    analogWrite(ledColorPins[0], red);
    analogWrite(ledColorPins[1], green);
    analogWrite(ledColorPins[2], blue);
}

void returnToMainMenu(){
    option = NOT_OPTION;
    subMenu = NOT_SUBMENU;
    mainMenuInput = defaultInput;
    printMainMenu();
}

void exitOption(){
    if(Serial.available()){
        char cmd = Serial.read();
        if(cmd == exitOptionChar){
            returnToMainMenu();
        }
        else {
            printExitKey();
        }
    }
    
}

void resetLoggerData(){
    for(int i = 0; i < numSensorLogs * logValueSize * numSensors; i++){
        EEPROM.put(i, defaultEEPROMValue);
    }
}

// all print functions
// unordered unfortunately

void printCurrentSensorSettings(){
    Serial.print(F("Sampling Interval: "));
    Serial.print(intEEPROMGet(samplingIntervalAddress) / 1000);
    Serial.print(F(" s\nUltrasonic Alert Threshold: "));
    Serial.print(intEEPROMGet(distanceAlertThresholdAddress));
    Serial.print(F(" cm\nLDR Alert Threshold: "));
    Serial.print(intEEPROMGet(ldrAlertThresholdAddress));
    Serial.println(F("%"));
}

void printIndexLoggedData(int index){
    Serial.print(F("Ultrasonic Reading: "));
    Serial.print(intEEPROMGet(index));
    Serial.print(F(" cm, LDR Reading: "));
    Serial.print(intEEPROMGet(index + numSensorLogs * logValueSize));
    Serial.println(F("%"));
}

void printAllLoggedData(){
    Serial.println(F("Logged Data:"));
    int count = numSensorLogs;
    for(int i = prevLogIndex; i >= 0; i -= logValueSize){
        printIndexLoggedData(i);
        count--;
    }
    for(int i = numSensorLogs * logValueSize - logValueSize; i >= 0 && count > 0; i -= logValueSize){
        printIndexLoggedData(i);
        count--;
    }
}

void printExitKey(){
    Serial.print(F("Send <"));
    Serial.print(exitOptionChar);
    Serial.println(F("> to exit option"));
}


void printMainMenu(){
    Serial.println();
    Serial.println(F("1. Sensor Settings"));
    Serial.println(F("2. Reset Logger Data"));
    Serial.println(F("3. System Status"));
    Serial.println(F("4. RGB LED Control"));
}

void printSensorSettingsMenu(){
    Serial.println();
    Serial.println(F("1.1. Sensors Sampling Interval"));
    Serial.println(F("1.2. Ultrasonic Alert Threshold"));
    Serial.println(F("1.3. LDR Alert Threshold"));
    Serial.println(F("1.4. Back"));
}

void printResetLoggerDataMenu(){
    Serial.println();
    Serial.println(F("Are you sure you want to reset the logger data?"));
    Serial.println(F("2.1. Yes"));
    Serial.println(F("2.2. No"));
}

void printSystemStatusMenu(){
    Serial.println();
    Serial.println(F("3.1. Current Sensor Readings"));
    Serial.println(F("3.2. Current Sensor Settings"));
    Serial.println(F("3.3. Display Logged Data"));
    Serial.println(F("3.4. Back"));
}

void printRGBLEDControlMenu(){
    Serial.print(F("4.1. Manual Color Control | Currently: "));
    Serial.print(EEPROM.read(redLEDAddress));
    Serial.print(F(" "));
    Serial.print(EEPROM.read(redLEDAddress + 1));
    Serial.print(F(" "));
    Serial.println(EEPROM.read(redLEDAddress + 2));

    Serial.print(("4.2. LED: Toggle Automatic ON/OFF | Currently: "));
    Serial.println(intEEPROMGet(isAutomaticRGBAddress) ? "ON" : "OFF");

    Serial.println(F("4.3. Back"));
}

// input checking functions
// any function named check*Input() is referring to the input of the menu/submenu/option
// and the switching of the menu/submenu/option

// not very organized

void checkMainMenuInput(){
    if(Serial.available()){
        mainMenuInput = Serial.parseInt();
        switch(mainMenuInput){
            case SENSOR_SETTINGS:
                printSensorSettingsMenu();
                subMenu = SENSOR_SETTINGS;
                break;
            case RESET_LOGGER_DATA:
                printResetLoggerDataMenu();
                subMenu = RESET_LOGGER_DATA;
                break;
            case SYSTEM_STATUS:
                printSystemStatusMenu();
                subMenu = SYSTEM_STATUS;
                break;
            case RGB_LED_CONTROL:
                printRGBLEDControlMenu();
                subMenu = RGB_LED_CONTROL;
                break;
            default:
                Serial.println(invalidInput);
                break;
        }
    }
}

void checkSubMenuInput(){
    switch(mainMenuInput){
        case SENSOR_SETTINGS:
            checkSensorSettingsInput();
            break;
        case RESET_LOGGER_DATA:
            checkResetLoggerDataInput();
            break;
        case SYSTEM_STATUS:
            checkSystemStatusInput();
            break;
        case RGB_LED_CONTROL:
            checkRGBLEDControlInput();
            break;
        default:
            Serial.println(invalidInput);
            break;
    }
}

void checkSensorSettingsInput(){
    if(Serial.available()){
        subMenuInput = Serial.parseInt();
        switch(subMenuInput){
            case SAMPLING_INTERVAL:
                Serial.println(F("Input new sampling interval in seconds:"));
                option = SET_SAMPLING_INTERVAL;
                break;
            case DISTANCE_ALERT_THRESHOLD:
                Serial.println(F("Input new ultrasonic alert threshold:"));
                option = SET_DISTANCE_ALERT_THRESHOLD;
                break;
            case LDR_ALERT_THRESHOLD:
                Serial.println(F("Input new LDR alert threshold:"));
                option = SET_LDR_ALERT_THRESHOLD;
                break;
            case SENSOR_SETTINGS_EXIT:
                returnToMainMenu();
                break;
            default:
                Serial.println(invalidInput);
                printSensorSettingsMenu();
                break;
        }
    }
}

void checkResetLoggerDataInput(){
    if(Serial.available()){
        subMenuInput = Serial.parseInt();
        switch(subMenuInput){
            case YES:
                resetLoggerData();
                returnToMainMenu();
                break;
            case NO:
                returnToMainMenu();
                break;
            default:
                Serial.println(invalidInput);
                printResetLoggerDataMenu();
                break;
        }
    }
}

void checkSystemStatusInput(){
    if(Serial.available()){
        subMenuInput = Serial.parseInt();
        switch(subMenuInput){
            case CURRENT_SENSOR_READINGS:
                option = PRINT_SENSOR_READINGS;
                printExitKey();
                break;
            case CURRENT_SENSOR_SETTINGS:
                printCurrentSensorSettings();
                returnToMainMenu();
                break;
            case DISPLAY_LOGGED_DATA:
                printAllLoggedData();
                returnToMainMenu();
                break;
            case SYSTEM_STATUS_EXIT:
                returnToMainMenu();
                break;
            default:
                Serial.println(invalidInput);
                printSystemStatusMenu();
                break;
        }
    }
}

void checkRGBLEDControlInput(){
    if(Serial.available()){
        subMenuInput = Serial.parseInt();
        switch(subMenuInput){
            case MANUAL_COLOR_CONTROL_INPUT:
                option = MANUAL_COLOR_CONTROL;
                Serial.println(F("Input RGB values (0-255) separated by spaces:"));
                break;
            case TOGGLE_AUTOMATIC_RGB:
                intEEPROMGet(isAutomaticRGBAddress) ? EEPROM.put(isAutomaticRGBAddress, 0) : EEPROM.put(isAutomaticRGBAddress, 1);
                returnToMainMenu();
                break;
            case RGB_LED_CONTROL_EXIT:
                returnToMainMenu();
                break;
            default:
                Serial.println(invalidInput);
                printRGBLEDControlMenu();
                break;
        }
    }
}

void checkOptionInput(){
    switch(option){
        case PRINT_SENSOR_READINGS:
            exitOption();
            break;
        case SET_SAMPLING_INTERVAL:
            setSetting(minSamplingInterval, maxSamplingInterval, samplingIntervalAddress, "New Sampling Interval: ", " s", true);
            break;
        case SET_DISTANCE_ALERT_THRESHOLD:
            setSetting(minDistanceAlertThreshold, maxDistanceAlertThreshold, distanceAlertThresholdAddress, "New Ultrasonic Alert Threshold: ", " cm");
            break;
        case SET_LDR_ALERT_THRESHOLD:
            setSetting(minLDRAlertThreshold, maxLDRAlertThreshold, ldrAlertThresholdAddress, "New LDR Alert Threshold: ", " %");
            break;
        case MANUAL_COLOR_CONTROL:
            checkRGBValuesInput();
            break;
        default:
            Serial.println(invalidInput);
            break;
    }
}

void setSetting(int min, int max, int address, char settingPrompt[], char unit[], bool convertToSeconds = false){
    if(Serial.available()){
        int newSetting = Serial.parseInt();
        if(newSetting >= min && newSetting <= max){
            EEPROM.put(address, newSetting * (convertToSeconds ? 1000 : 1));
            Serial.print(settingPrompt);
            Serial.print(newSetting);
            Serial.println(unit);
            returnToMainMenu();
        }
        else {
            Serial.println(invalidInput);
        }
    }
}

void checkRGBValuesInput(){
    if(Serial.available()){
        uint8_t redValue = Serial.parseInt();
        uint8_t greenValue = Serial.parseInt();
        uint8_t blueValue = Serial.parseInt();
        if(redValue >= minRGBValue && redValue <= maxRGBValue && greenValue >= minRGBValue && greenValue <= maxRGBValue && blueValue >= minRGBValue && blueValue <= maxRGBValue){
            setRGBLEDColor(redValue, greenValue, blueValue);
            Serial.print(F("New RGB Values: "));

            Serial.print(redValue);
            EEPROM.put(redLEDAddress, redValue);
            Serial.print(F(" "));

            Serial.print(greenValue);
            EEPROM.put(redLEDAddress + 1, greenValue);
            Serial.print(F(" "));

            Serial.println(blueValue);
            EEPROM.put(redLEDAddress + 2, blueValue);

            returnToMainMenu();
        }
        else {
            Serial.println(invalidInput);
        }
    }
}