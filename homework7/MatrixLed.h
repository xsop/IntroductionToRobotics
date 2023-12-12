#pragma once
#include "Init.h"
#include "LedControl.h" // MAX7219 library

class Matrix {
public:
    Matrix();
    
    const int getMatrixSize() {return matrixSize;};
    void setupMatrix();
    void resetMemory();
    void setLed(byte row, byte col, bool state, bool updateMatrix = 1);
    bool getLed(byte row, byte col) const;
    int getMemoryCounter() const { return memoryCounter; }
    void setBrightness(int brightness);
    int getBrightness();
private:
    LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER
    int memoryMatrix[matrixSize][matrixSize];
    int memoryCounter = 0;
};

