// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "ToneController.h"

class BuzzerToneController : public ToneController {
public:
    explicit BuzzerToneController(uint8_t pin);

    void begin() override;
    void tone(Tone tone, Duration duration) override;
};