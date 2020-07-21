#pragma once

#include "Definitions.h"

enum results initLEDs();
enum results initGPIO(int);
void IOsetLED(int, int);
void MainLoopDelay(int);
