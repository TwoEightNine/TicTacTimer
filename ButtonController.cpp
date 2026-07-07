#include "ButtonController.h"

ButtonController::ButtonController(uint8_t pin, ToneController& tone)
    : pin(pin),
      tone(tone),
      pendingEvent(NONE) {}

void ButtonController::begin() {
    pinMode(pin, INPUT_PULLDOWN);
}

void ButtonController::tick() {
    bool pressed = digitalRead(pin);
    unsigned long now = millis();

    switch (state) {

        case IDLE:
            if (pressed) {
                state = DEBOUNCING;
                stateStarted = now;
            }
            break;

        case DEBOUNCING:
            if (!pressed) {
                state = IDLE;
            }
            else if (now - stateStarted >= DEBOUNCE_MS) {
                state = PRESSED;
                pressStarted = stateStarted;
            }
            break;

        case PRESSED:
            if (!pressed) {
                unsigned long duration = now - pressStarted;
                if (duration >= EXTRA_LONG_CLICK_MS) {
                    pendingEvent = EXTRA_LONG_CLICK;
                } else if (duration >= LONG_CLICK_MS) {
                    pendingEvent = LONG_CLICK;
                } else {
                    pendingEvent = SHORT_CLICK;
                }
                state = IDLE;
            }
            break;
    }
}

ButtonController::Event ButtonController::getEvent() {
    Event e = pendingEvent;
    pendingEvent = NONE;
    return e;
}