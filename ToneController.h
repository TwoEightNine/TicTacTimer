#pragma once

#include <Arduino.h>

class ToneController {
public:
    enum Tone {
        HALF,
        KILO,
        CHIRP
    };

    enum Duration {
        TICK,
        SHORT,
        MEDIUM,
        LONG
    };

    explicit ToneController(uint8_t pin)
        : pin(pin){}

    virtual ~ToneController() = default;

    virtual void begin() = 0;
    virtual void tone(Tone tone, Duration duration) = 0;

protected:
    uint8_t pin;
};