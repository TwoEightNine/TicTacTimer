// SPDX-License-Identifier: GPL-3.0-or-later
#include "NoopToneController.h"

NoopToneController::NoopToneController(uint8_t pin)
    : ToneController(pin){}

void NoopToneController::begin(){}

void NoopToneController::tone(Tone tone, Duration duration) {}