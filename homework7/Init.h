#pragma once

#include "EEPROM.h"
#include "Consts.h"
#include "MatrixLed.h"
#include "Entity.h"
#include "GameController.h"
#include "Map.h"
#include "LCD.h"

Matrix matrix;
Player player;
Controller controller;
GameMap gameMap;
Display display;

bool isInGame = false;

byte cursorPos = 0;
byte pagePos = 0;
bool changePrint = true;
bool isInMain = true;
bool isInSettings = false;
bool isInAbout = false;
bool isInValueInput = false;
byte currentValue = 0;
bool isInGameOver = false;

void fullMatrixOn();
void startGame();
void updateGame();

void menu();