#pragma once
#include "Init.h"
#include "LedControl.h" // MAX7219 library

class Matrix {
public:
    Matrix();
    const int getMatrixSize() {return matrixSize;};
    void setupMatrix();
    void setLed(byte row, byte col, bool state, bool updateMatrix = 1);
    bool getLed(byte row, byte col) const;
private:
    const byte dinPin = 12; // pin 12 is connected to the MAX7219 pin 1
    const byte clockPin = 11; // pin 11 is connected to the CLK pin 13
    const byte loadPin = 10; // pin 10 is connected to LOAD pin 12
    const byte matrixSize = 8; // 1 as we are only using 1 MAX7219
    LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER
    byte matrixBrightness = 2;

    int memoryMatrix[8][8];
};

