#pragma once

#include "ToneController.h"

class BuzzerToneController : public ToneController {
public:
    explicit BuzzerToneController(uint8_t pin);

    void begin() override;
    void tone(Tone tone, Duration duration) override;
};