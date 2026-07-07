#pragma once

#include "Screen.h"

Screen* createMenuScreen(Adafruit_SSD1306& display, ToneController& tone, ButtonController& button, Navigator* navigator);