#pragma once

#include "Screen.h"

Screen* createWorkoutScreen(Adafruit_SSD1306& display, ToneController& tone, ButtonController& button, Navigator* navigator);