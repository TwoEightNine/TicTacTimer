#pragma once

#include "Screen.h"

Screen* createSplashScreen(Adafruit_SSD1306& display, ToneController& tone, ButtonController& button, Navigator* navigator);