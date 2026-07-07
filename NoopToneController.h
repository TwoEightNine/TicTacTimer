#pragma once

#include "ToneController.h"

class NoopToneController : public ToneController {
public:
    explicit NoopToneController(uint8_t pin);

    void begin() override;
    void tone(Tone tone, Duration duration) override;
};