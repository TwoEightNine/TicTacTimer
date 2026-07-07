#include "BuzzerToneController.h"

BuzzerToneController::BuzzerToneController(uint8_t pin)
    : ToneController(pin)
{
}

void BuzzerToneController::begin()
{
    pinMode(pin, OUTPUT);
}

void BuzzerToneController::tone(Tone tone, Duration duration) {
    uint16_t frequency = 500;
    if (tone == KILO) {
      frequency = 1000;
    } else if (tone == CHIRP) {
      frequency = 1500;
    }

    uint16_t durationMs = 62;
    if (duration == SHORT) {
      durationMs = 125;
    } else if (duration == MEDIUM) {
      durationMs = 250;
    } else if (duration == LONG) {
      durationMs = 500;
    }
    ::tone(pin, frequency, durationMs);
}