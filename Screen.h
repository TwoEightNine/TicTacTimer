#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ToneController.h"
#include "ButtonController.h"

class Navigator;

class Screen {
public:
    explicit Screen(Adafruit_SSD1306& display, ToneController& tone, ButtonController& button, Navigator* navigator)
        : display(display), 
        tone(tone), 
        button(button), 
        navigator(navigator) {}

    virtual ~Screen() = default;

    virtual void tick() = 0;

protected:
    Adafruit_SSD1306& display;
    ToneController& tone;
    ButtonController& button;
    Navigator* navigator;
};