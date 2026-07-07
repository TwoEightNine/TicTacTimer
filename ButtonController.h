#pragma once

#include <Arduino.h>
#include "ToneController.h"

class ButtonController {
public:
    enum Event {
        NONE,
        SHORT_CLICK,
        LONG_CLICK,
        EXTRA_LONG_CLICK
    };

    explicit ButtonController(uint8_t pin, ToneController& tone);

    void begin();
    void tick();

    Event getEvent();

private:
    uint8_t pin;
    ToneController& tone;

    Event pendingEvent;

    enum State {
        IDLE,
        DEBOUNCING,
        PRESSED
    };

    State state = IDLE;

    unsigned long stateStarted = 0;
    unsigned long pressStarted = 0;

    static constexpr unsigned long DEBOUNCE_MS = 30;
    static constexpr unsigned long LONG_CLICK_MS = 700;
    static constexpr unsigned long EXTRA_LONG_CLICK_MS = 2000;
};