// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../core/Screen.h"

Screen* createFlappyBirdScreen(
    Adafruit_SSD1306& display,
    ToneController& tone,
    ButtonController& button,
    Navigator* navigator
);